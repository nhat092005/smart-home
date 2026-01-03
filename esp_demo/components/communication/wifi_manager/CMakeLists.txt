idf_component_register(
     SRCS
     "wifi_manager.c"
     "dns_server.c"
     INCLUDE_DIRS "include"
     REQUIRES
     esp_wifi
     esp_netif
     esp_event
     nvs_flash
     webserver
)
