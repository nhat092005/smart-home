/**
 * @file task_manager.c
 *
 * @brief Task Manager Implementation
 */

#include "task_manager.h"

/* Global state flags (for LED status display) */
// bool isModeON = false; // Defined in mode_manager.c
// bool isWiFi = false;   // Defined in wifi_manager.c
// bool isMQTT = false;   // Defined in mqtt_manager.c

/* Exported variables definitions */
char g_app_version[16] = VERSION_APP;           //!< Application version string
uint32_t g_interval_time_ms = INTERVAL_TIME_MS; //!< Data publish interval in milliseconds
