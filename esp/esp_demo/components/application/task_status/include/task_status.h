/**
 * @file task_status.h
 *
 * @brief Task Status Header API
 */

#ifndef TASK_STATUS_H
#define TASK_STATUS_H

/* Includes -------------------------------------------------------------------*/

#include "esp_err.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Task polling LED
 */
esp_err_t task_status_set_init(void);

#endif /* TASK_STATUS_H */