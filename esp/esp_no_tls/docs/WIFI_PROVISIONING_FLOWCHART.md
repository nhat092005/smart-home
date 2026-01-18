# LƯU ĐỒ XỬ LÝ WIFI - PROVISIONING & CONNECTION

## Tổng quan

Lưu đồ này mô tả quy trình hoàn chỉnh xử lý WiFi của hệ thống Smart Home ESP32, bao gồm:
- Kiểm tra credentials đã lưu
- Chế độ Provisioning (tạo Access Point + Captive Portal)
- Kết nối WiFi
- Khởi động MQTT

---

## 1. LƯU ĐỒ TỔNG QUAN - MAIN FLOW

```
                        +---------------------------+
                        |      KHỞI ĐỘNG HỆ THỐNG  |
                        |    (System Startup)       |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        |  wifi_manager_init()      |
                        |  - Khởi tạo NVS          |
                        |  - Tạo event handlers     |
                        |  - Load credentials       |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        |  wifi_manager_start()     |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        | Đã có credentials lưu?    |
                        | (provisioned == true?)    |
                        +------------+--------------+
                                     |
                    +----------------+----------------+
                    |                                 |
                    | NO                              | YES
                    v                                 v
    +-------------------------------+     +---------------------------+
    |  CHƯA CÓ CREDENTIALS          |     |  ĐÃ CÓ CREDENTIALS        |
    |                               |     |                           |
    |  wifi_manager_start_         |     |  wifi_manager_connect()   |
    |  provisioning()               |     |                           |
    +---------------+---------------+     +-------------+-------------+
                    |                                   |
                    v                                   v
    +-------------------------------+     +---------------------------+
    |  CHẾ ĐỘ PROVISIONING          |     |  CHẾ ĐỘ STATION (STA)     |
    |  (Access Point Mode)          |     |  (Kết nối WiFi)           |
    +---------------+---------------+     +-------------+-------------+
                    |                                   |
                    |                                   |
                    v                                   v
    +-------------------------------+     +---------------------------+
    |  1. Tạo WiFi AP               |     |  1. Kết nối WiFi          |
    |     SSID: SmartHome_Setup    |     |     (với credentials)     |
    |     IP: 192.168.4.1          |     |                           |
    |  2. Start DNS Server          |     |  2. Đợi CONNECTED         |
    |     (Captive Portal)          |     |                           |
    |  3. Start Web Server          |     |  3. Đợi GOT_IP            |
    |     (Port 80)                 |     |                           |
    +---------------+---------------+     +-------------+-------------+
                    |                                   |
                    v                                   |
    +-------------------------------+                   |
    |  Người dùng kết nối vào AP    |                   |
    |  192.168.4.1                  |                   |
    +---------------+---------------+                   |
                    |                                   |
                    v                                   |
    +-------------------------------+                   |
    |  DNS Server chuyển hướng      |                   |
    |  tất cả DNS queries →         |                   |
    |  192.168.4.1 (Captive Portal) |                   |
    +---------------+---------------+                   |
                    |                                   |
                    v                                   |
    +-------------------------------+                   |
    |  Web Server hiển thị:         |                   |
    |  - Danh sách WiFi             |                   |
    |  - Form nhập password         |                   |
    +---------------+---------------+                   |
                    |                                   |
                    v                                   |
    +-------------------------------+                   |
    |  Người dùng chọn WiFi         |                   |
    |  và nhập password             |                   |
    +---------------+---------------+                   |
                    |                                   |
                    v                                   |
    +-------------------------------+                   |
    |  Lưu credentials vào NVS      |                   |
    |  (ssid + password)            |                   |
    +---------------+---------------+                   |
                    |                                   |
                    v                                   |
    +-------------------------------+                   |
    |  PROVISIONING_SUCCESS event   |                   |
    |  Khởi động lại ESP32          |                   |
    +---------------+---------------+                   |
                    |                                   |
                    +-----------------------------------+
                                     |
                                     v
                        +---------------------------+
                        |  WIFI CONNECTED           |
                        |  (got IP address)         |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        |  WIFI_EVENT_GOT_IP        |
                        |  callback triggered       |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        |  task_wifi.c:             |
                        |  wifi_event_callback()    |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        |  Log IP information:      |
                        |  - IP address             |
                        |  - Gateway                |
                        |  - Netmask                |
                        +------------+--------------+
                                     |
                                     v
                        +---------------------------+
                        |  mqtt_manager_start()     |
                        |  Khởi động MQTT Client    |
                        +------------+--------------+
                                     |
              +----------------------+----------------------+
              |                                             |
              | SUCCESS                                     | FAIL
              v                                             v
    +---------------------+                     +-----------------------+
    |  MQTT CONNECTED     |                     |  Log error            |
    |  Sẵn sàng gửi/nhận  |                     |  Tiếp tục retry       |
    |  MQTT messages      |                     +-----------------------+
    +---------------------+


    +------------------------+
    |  HỆ THỐNG HOẠT ĐỘNG   |
    |  BÌNH THƯỜNG          |
    +------------------------+
```

---

## 2. LƯU ĐỒ CHI TIẾT - PROVISIONING MODE

```
                    +---------------------------+
                    |  wifi_manager_start_      |
                    |  provisioning()           |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Set WiFi Mode:           |
                    |  WIFI_MODE_APSTA          |
                    |  (AP + STA đồng thời)     |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Cấu hình AP:             |
                    |  - SSID: SmartHome_Setup  |
                    |  - Channel: 1             |
                    |  - Auth: OPEN (no pass)   |
                    |  - Max Conn: 4            |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  esp_wifi_start()         |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  State → PROVISIONING     |
                    +------------+--------------+
                                 |
          +----------------------+----------------------+
          |                                             |
          v                                             v
+---------------------+                     +------------------------+
|  webserver_start()  |                     |  dns_server_start()    |
|                     |                     |                        |
|  - Port: 80         |                     |  - Port: 53            |
|  - Routes:          |                     |  - Task: 4KB stack     |
|    /                |                     |  - Redirect all DNS    |
|    /scan            |                     |    queries → AP IP     |
|    /connect         |                     +------------------------+
+---------------------+
          |
          v
+---------------------+
|  CAPTIVE PORTAL     |
|  ACTIVE             |
|                     |
|  Người dùng có thể  |
|  truy cập web UI    |
+---------------------+
          |
          v
+---------------------+
|  Web UI Routes:     |
|                     |
|  GET /              |
|  → index.html       |
|                     |
|  GET /scan          |
|  → JSON AP list     |
|                     |
|  POST /connect      |
|  → Save & restart   |
+---------------------+
```

---

## 3. LƯU ĐỒ CHI TIẾT - WEB SCAN & CONNECT FLOW

```
                    +---------------------------+
                    |  User mở browser          |
                    |  http://192.168.4.1       |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  GET / (index.html)       |
                    |  Hiển thị UI setup        |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  JavaScript gọi:          |
                    |  GET /scan                |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Server thực hiện:        |
                    |  wifi_manager_scan_       |
                    |  networks()               |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Scan WiFi networks       |
                    |  (tìm tất cả AP xung      |
                    |  quanh, max 16 AP)        |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Trả về JSON:             |
                    |  [                        |
                    |    {ssid: "xxx",          |
                    |     rssi: -45,            |
                    |     auth: WPA2},          |
                    |    ...                    |
                    |  ]                        |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  UI hiển thị danh sách    |
                    |  WiFi networks            |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  User chọn WiFi           |
                    |  và nhập password         |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Click "Connect"          |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  POST /connect            |
                    |  {                        |
                    |    ssid: "xxx",           |
                    |    password: "yyy"        |
                    |  }                        |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Server lưu vào NVS:      |
                    |  wifi_manager_save_       |
                    |  credentials()            |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Trả về response:         |
                    |  {status: "success"}      |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  PROVISIONING_SUCCESS     |
                    |  event triggered          |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Log: "Device will        |
                    |  restart to connect..."   |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  esp_restart()            |
                    |  Khởi động lại ESP32      |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Sau khi khởi động:       |
                    |  - Load credentials       |
                    |  - provisioned = true     |
                    |  - Kết nối WiFi bình      |
                    |    thường (STA mode)      |
                    +---------------------------+
```

---

## 4. LƯU ĐỒ CHI TIẾT - STATION MODE CONNECTION

```
                    +---------------------------+
                    |  wifi_manager_connect()   |
                    |  (ssid, password)         |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Set WiFi Mode:           |
                    |  WIFI_MODE_STA            |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Cấu hình STA:            |
                    |  - SSID                   |
                    |  - Password               |
                    |  - Auth threshold         |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  esp_wifi_start()         |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  esp_wifi_connect()       |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  State → CONNECTING       |
                    |  isWiFiConnecting = true  |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  LED nhấp nháy 250ms      |
                    |  (wifi_connecting_task)   |
                    +------------+--------------+
                                 |
          +----------------------+----------------------+
          |                                             |
          | Kết nối thành công                          | Kết nối thất bại
          v                                             v
+---------------------------+             +---------------------------+
|  WIFI_EVENT_CONNECTED     |             |  WIFI_EVENT_DISCONNECTED  |
+------------+--------------+             +------------+--------------+
             |                                         |
             v                                         v
+---------------------------+             +---------------------------+
|  Đợi nhận IP từ DHCP      |             |  retry_count++            |
+------------+--------------+             +------------+--------------+
             |                                         |
             v                                         v
+---------------------------+             +---------------------------+
|  IP_EVENT_STA_GOT_IP      |             |  retry < MAX_RETRY?       |
+------------+--------------+             +------------+--------------+
             |                                         |
             v                              +----------+----------+
+---------------------------+               |                     |
|  WIFI_EVENT_GOT_IP        |               | YES                 | NO
|  callback                 |               v                     v
+------------+--------------+     +----------------+    +------------------+
             |                    |  Retry connect |    |  Stop connecting |
             v                    +----------------+    |  Log error       |
+---------------------------+                           +------------------+
|  isWiFiConnecting = false |
|  LED ngừng nhấp nháy      |
+------------+--------------+
             |
             v
+---------------------------+
|  Log thông tin:           |
|  - IP: 192.168.1.X        |
|  - Gateway                |
|  - Netmask                |
+------------+--------------+
             |
             v
+---------------------------+
|  mqtt_manager_start()     |
+------------+--------------+
             |
+------------v-------------+
|  MQTT kết nối broker     |
|  → System ready          |
+--------------------------+
```

---

## 5. LƯU ĐỒ XỬ LÝ SỰ KIỆN - EVENT CALLBACK

```
                    +---------------------------+
                    |  WiFi/IP Event xảy ra     |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  wifi_event_handler()     |
                    |  (trong wifi_manager.c)   |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Xử lý event và update    |
                    |  state machine            |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Gọi registered callback  |
                    |  g_wifi_ctx.callback()    |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  task_wifi_event_         |
                    |  callback()               |
                    |  (trong task_wifi.c)      |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  Switch (event_type)      |
                    +------------+--------------+
                                 |
    +----------+----------+------+------+----------+----------+
    |          |          |             |          |          |
    v          v          v             v          v          v
+--------+ +--------+ +--------+ +----------+ +--------+ +--------+
|DISCONN | |CONNEC  | |CONNEC  | |  GOT_IP  | |PROV_   | |PROV_   |
|ECTED   | |TING    | |TED     | |          | |STARTED | |SUCCESS |
+---+----+ +---+----+ +---+----+ +----+-----+ +---+----+ +---+----+
    |          |          |           |           |          |
    v          v          v           v           v          v
Log warn   Set LED   Log info   Start MQTT   Log AP    Restart
           blink                              info      device
```

---

## 6. LƯU ĐỒ NẾU KẾT NỐI THẤT BẠI

```
                    +---------------------------+
                    |  Kết nối WiFi thất bại    |
                    |  (wrong password, AP      |
                    |   không tồn tại, etc)     |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  WIFI_EVENT_DISCONNECTED  |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  retry_count++            |
                    +------------+--------------+
                                 |
                                 v
                    +---------------------------+
                    |  retry_count < MAX_RETRY? |
                    |  (MAX_RETRY = 5)          |
                    +------------+--------------+
                                 |
              +------------------+------------------+
              |                                     |
              | YES (< 5)                           | NO (≥ 5)
              v                                     v
    +----------------------+            +-------------------------+
    |  Chờ 1s              |            |  Log error: "Failed to  |
    |  esp_wifi_connect()  |            |  connect after retries" |
    |  (thử lại)           |            +------------+------------+
    +----------------------+                         |
              |                                      v
              |                          +-------------------------+
              |                          |  OPTION 1:              |
              +------------------------->|  Giữ nguyên trạng thái  |
                                         |  (không kết nối)        |
                                         +-------------------------+
                                                     |
                                                     v
                                         +-------------------------+
                                         |  OPTION 2:              |
                                         |  Tự động chuyển sang    |
                                         |  Provisioning mode để   |
                                         |  user cấu hình lại      |
                                         +-------------------------+
                                                     |
                                                     v
                                         +-------------------------+
                                         |  User có thể:           |
                                         |  - Reset ESP32          |
                                         |  - Clear credentials    |
                                         |  - Provisioning lại     |
                                         +-------------------------+
```

---

## 7. SƠ ĐỒ HỆ THỐNG - COMPONENT INTERACTION

```
+-----------------------------------------------------+
|                   MAIN APPLICATION                  |
|                      (main.c)                       |
+------------------+----------------------------------+
                   |
                   | Initialize all managers
                   |
    +--------------v--------------+
    |                             |
    v                             v
+-------------------+    +------------------+
|  WiFi Manager     |    |  Task WiFi       |
|  Component        |    |  Component       |
|                   |    |                  |
|  - init()         |    |  - callback      |
|  - start()        |    |  - LED blink     |
|  - provisioning() |<---+  - MQTT trigger |
|  - connect()      |    |                  |
+--------+----------+    +------------------+
         |
         | Start web/DNS
         |
    +----v---------+
    |              |
    v              v
+----------+  +-----------+
|Webserver |  |DNS Server |
|          |  |           |
|- Routes  |  |- Port 53  |
|  /scan   |  |- Captive  |
|  /connect|  |  portal   |
+----+-----+  +-----------+
     |
     | On credentials saved
     |
     v
+--------------------+
|   NVS Storage      |
|                    |
|  - wifi_creds      |
|    - ssid          |
|    - password      |
+--------------------+
     |
     | Load on boot
     |
     v
(Back to WiFi Manager)


After WiFi connected:
+--------------------+
|   MQTT Manager     |
|                    |
|  - connect broker  |
|  - pub/sub topics  |
+--------------------+
```

---

## 8. TIMELINE - QUY TRÌNH THỰC TẾ

### Scenario 1: Lần đầu khởi động (chưa có credentials)

```
T=0s     System boot → wifi_manager_init()
T=0.5s   Load NVS → No credentials found
T=1s     Start Provisioning Mode
         - Create AP "SmartHome_Setup"
         - Start DNS server (port 53)
         - Start Web server (port 80)
T=2s     AP ready at 192.168.4.1
         → LED OFF (provisioning mode)
         
[User connects to AP]
T=30s    User connected to AP
T=32s    User opens browser → Redirected to 192.168.4.1
T=35s    User clicks "Scan WiFi"
T=37s    Server returns list of networks
T=40s    User selects WiFi and enters password
T=42s    User clicks "Connect"
T=43s    Credentials saved to NVS
T=44s    PROVISIONING_SUCCESS event
T=45s    ESP32 restarts...

[After restart]
T=0s     System boot → wifi_manager_init()
T=0.5s   Load NVS → Credentials found!
T=1s     wifi_manager_connect()
         - Set WIFI_MODE_STA
         - Start connecting
         → LED blinks 250ms (connecting)
T=3s     WIFI_EVENT_CONNECTED
T=4s     IP_EVENT_STA_GOT_IP
         - IP: 192.168.1.100
         → LED stops blinking
T=5s     mqtt_manager_start()
T=6s     MQTT connected to broker
         → System ready!
```

### Scenario 2: Khởi động với credentials đã lưu

```
T=0s     System boot → wifi_manager_init()
T=0.5s   Load NVS → Credentials found
T=1s     wifi_manager_connect()
         → LED blinks (connecting)
T=3s     WIFI_EVENT_CONNECTED
T=4s     IP_EVENT_STA_GOT_IP
         → LED stops blinking
T=5s     mqtt_manager_start()
T=6s     MQTT connected
         → System ready!
```

---

## 9. THAM SỐ CẤU HÌNH

| Parameter | Value | Description |
|-----------|-------|-------------|
| AP SSID | SmartHome_Setup | Tên AP trong provisioning mode |
| AP IP | 192.168.4.1 | IP của ESP32 khi ở chế độ AP |
| AP Password | (none) | Không có password (OPEN) |
| AP Max Connections | 4 | Tối đa 4 clients kết nối cùng lúc |
| Web Server Port | 80 | HTTP port |
| DNS Server Port | 53 | DNS port cho captive portal |
| Max Retry | 5 | Số lần thử kết nối lại |
| LED Blink Interval | 250ms | Chu kỳ nhấp nháy khi connecting |
| Task Stack Size | 2048 bytes | Stack cho wifi_connecting_task |

---

## 10. LƯU Ý QUAN TRỌNG

### ✅ Điểm mạnh của thiết kế này:

1. **Tự động Provisioning**: Nếu chưa có credentials → tự động tạo AP
2. **Captive Portal**: DNS server tự động redirect → UX tốt
3. **Thread-safe**: Sử dụng mutex, semaphore, event groups
4. **Visual Feedback**: LED nhấp nháy cho biết trạng thái
5. **Persistent Storage**: Lưu credentials vào NVS
6. **Event-driven**: Kiến trúc callback rõ ràng
7. **MQTT Auto-start**: Tự động khởi động MQTT sau khi có IP

### ⚠️ Cần lưu ý:

1. **Security**: AP mode không có password (open network)
2. **Credentials**: Password lưu plaintext trong NVS
3. **Retry Logic**: Sau 5 lần thất bại → cần user can thiệp
4. **DNS Overhead**: DNS server chạy task riêng (4KB stack)
5. **Memory**: Provisioning mode cần nhiều RAM (AP + Web + DNS)

---

## KẾT LUẬN

Đây là lưu đồ hoàn chỉnh mô tả cách hệ thống xử lý WiFi. Quy trình này cho phép:

1. ✅ Device tự động provisioning nếu chưa có WiFi
2. ✅ User dễ dàng cấu hình qua web interface
3. ✅ Tự động kết nối WiFi khi có credentials
4. ✅ Tự động khởi động MQTT sau khi có IP
5. ✅ Visual feedback qua LED
6. ✅ Retry logic khi kết nối thất bại

**Các file liên quan:**
- `components/communication/wifi_manager/wifi_manager.c` - Logic chính
- `components/communication/wifi_manager/dns_server.c` - Captive portal
- `components/communication/webserver/` - Web UI
- `components/application/task_wifi/task_wifi.c` - Event callbacks

**Luồng quan trọng nhất**: 
`Boot → Check NVS → Provisioning/Connect → Got IP → Start MQTT → Ready!`
