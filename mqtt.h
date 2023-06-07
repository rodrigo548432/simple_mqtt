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


typedef struct {
    int qos;
    int topic_len;
    char *topic;
} subscribe_parameters;

/**
 * @brief This function initializes MQTT connection. Create socket and send
 * connection message packet, expects a valid connack answer.
 * @param hostname MQTT server hostname.
 * @param port MQTT server port.
 * @param clientID Client identification.
 * @param username Username to authenticate to MQTT Broker.
 * @param password Password to authenticate to MQTT Broker.
 * @return MQTT socket handler or -1 if error.
 */
int mqtt_connect(const char *hostname,
                    int port,
                    const char *clientID,
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
 * @param topic MQTT topic to publish.
 * @param msg Message to publish
 * @return 0 if success or -1 if error.
 */
int mqtt_publish(int mqtt_socket, const char *topic, const char *msg);

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