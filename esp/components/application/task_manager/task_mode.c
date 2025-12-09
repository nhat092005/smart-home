/**
 * @file task_mode.c
 *
 * @brief Task Mode Implementation
 */

#include "task_manager.h"

void task_mode_change_event_callback(device_mode_t old_mode, device_mode_t new_mode)
{
    if (new_mode == MODE_ON)
    {
        // Logic for mode ON
    }
    else
    {
        // Logic for mode OFF
    }
}