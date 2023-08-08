/**
 * @file mqtt.h
 * @brief Main MQTT functions declaration.
 */

#ifndef _MQTT_H_
#define _MQTT_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define ENABLE_TRACES
#include "trace.h"


typedef enum {
    MQTT_CONNACK_ACCEPTED,
    MQTT_CONNACK_REFUSED_BAD_PROTOCOL,
    MQTT_CONNACK_REFUSED_ID_REJECTED,
    MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE,
    MQTT_CONNACK_REFUSED_BAD_USER_PASSWORD,
    MQTT_CONNACK_REFUSED_NOT_AUTHORIZED
} mqtt_connack_err_codes;

typedef enum {
    CONNECT_FLAG_RESERVED_BIT = 0b00000000,
    CONNECT_FLAG_CLEAN_SESSION = 0b00000010,
    CONNECT_FLAG_WILL = 0b00000100,
    CONNECT_FLAG_WILL_QOS_1 = 0b00001000,
    CONNECT_FLAG_WILL_QOS_2 = 0b00010000,
    CONNECT_FLAG_WILL_RETAIN = 0b00100000,
    CONNECT_FLAG_USERNAME = 0b01000000,
    CONNECT_FLAG_PASSWORD = 0b10000000
} mqtt_connect_flags;

typedef enum {
    PUBLISH_FLAG_QOS_1 = 0b00000000,
    PUBLISH_FLAG_RETAIN = 0b00000001,
    PUBLISH_FLAG_QOS_2 = 0b00000010,
    PUBLISH_FLAG_QOS_3 = 0b00000100,
    PUBLISH_FLAG_DUP = 0b0001000
} mqtt_publish_flags;

typedef enum {
    SUBSCRIBE_QOS_0 = 0,
    SUBSCRIBE_QOS_1,
    SUBSCRIBE_QOS_2
} mqtt_subscribe_qos;

typedef struct {
    mqtt_subscribe_qos qos;
    int topic_len;
    char *topic;
} subscribe_parameters;

/**
 * @brief This function initializes MQTT connection. Create socket and send
 * connection message packet, expects a valid connack answer.
 * @param hostname MQTT server hostname.
 * @param port MQTT server port.
 * @param clientID Client identification.
 * @param connection_flags Connection flags.
 * @param keepalive
 * @param username Username to authenticate to MQTT Broker.
 * @param password Password to authenticate to MQTT Broker.
 * @return MQTT socket handler or -1 if error.
 */
int mqtt_connect(const char *hostname,
                    int port,
                    const char *clientID,
                    mqtt_connect_flags conenction_flags,
                    int keepalive,
                    const char *username,
                    const char *password);

/**
 * @brief Simple mqtt connection function.
 * @param hostname MQTT server hostname.
 * @param port MQTT server port.
 * @param clientID Client identification.
 * @return MQTT socket handler or -1 if error.
 */
int mqtt_connect_simple(const char *hostname,
                            int port,
                            const char *clientID);

/**
 * @brief This function sends subscribe packet.
 * @param subs_params_len MQTT subscribe parameters array length.
 * @param subs_parameters MQTT subscribe parameters array.
 * @return 0 if success or -1 if error.
 */
int mqtt_subscribe(int mqtt_socket,
                    int subs_params_len,
                    subscribe_parameters *subs_parameters);

/**
 * @brief Publish message to topic.
 * @param mqtt_socket MQTT socket handler.
 * @param publish_flags Related flags to the related publish action.
 * @param topic MQTT topic to publish.
 * @param msg Message to publish.
 * @return 0 if success or -1 if error.
 */
int mqtt_publish(int mqtt_socket, mqtt_publish_flags publish_flags, 
                    const char *topic, const char *msg);

/**
 * @brief This function sends disconnect packet to MQTT Broker.
 * @param mqtt_socket MQTT socket handler.
 * @return None.
 */
void mqtt_disconnect(int mqtt_socket);

/**
 * @brief This function unsubscribe from topic.
 * @param mqtt_socket MQTT socket handler.
 * @param subs_params_len MQTT subscribe parameters array length.
 * @param subs_parameters MQTT subscribe parameters array.
 * @return 0 if success or -1 if error.
 */
int mqtt_unsubscribe(int mqtt_socket,
                        int subs_params_len,
                        subscribe_parameters *subs_parameters);

#endif /* _MQTT_H_ */