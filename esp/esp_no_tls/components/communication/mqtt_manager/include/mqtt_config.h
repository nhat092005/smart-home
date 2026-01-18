/**
 * @file mqtt_config.h
 *
 * @brief MQTT Manager Configuration from Kconfig
 */

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

/* Exported defines ----------------------------------------------------------*/

// Topic configuration
#define MQTT_BASE_TOPIC         CONFIG_MQTT_BASE_TOPIC

// Device ID from Kconfig
#define MQTT_DEVICE_ID          CONFIG_MQTT_DEVICE_ID

// Broker configuration
#define MQTT_BROKER_URI         CONFIG_MQTT_BROKER_URI
#define MQTT_BROKER_PORT        CONFIG_MQTT_BROKER_PORT
#define MQTT_USERNAME           CONFIG_MQTT_USERNAME
#define MQTT_PASSWORD           CONFIG_MQTT_PASSWORD

// MQTT settings

#define MQTT_QOS_0              0 // Fire and forget
#define MQTT_QOS_1              1 // At least once
#define MQTT_RETAIN_OFF         0
#define MQTT_RETAIN_ON          1
#define MQTT_KEEP_ALIVE_SEC     CONFIG_MQTT_KEEP_ALIVE_SEC

// Topic format strings - 4 Topics Structure
#define MQTT_TOPIC_DATA_FMT     "%s/%s/data"     //!< QoS=0, Retain=No
#define MQTT_TOPIC_STATE_FMT    "%s/%s/state"    //!< QoS=1, Retain=Yes
#define MQTT_TOPIC_INFO_FMT     "%s/%s/info"     //!< QoS=1, Retain=Yes
#define MQTT_TOPIC_COMMAND_FMT  "%s/%s/command"  //!< QoS=1, Retain=No
#define MQTT_TOPIC_RESPONSE_FMT "%s/%s/response" //!< QoS=1, Retain=Yes

#endif /* MQTT_CONFIG_H */