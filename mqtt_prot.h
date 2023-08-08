/**
 * @file mqtt_prot.h
 * @brief MQTT protocol functions declaration.
 * This header file and implementations are based on:
 * http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html
 */

#ifndef _MQTT_PROT_H_
#define _MQTT_PROT_H_

#include "stdio.h"
#include "string.h"
#include "stdint.h"

#define ENABLE_TRACES
#include "trace.h"

#define MQTT_PROT_PACKET_LEN 256

typedef enum {
    MQTT_PROT_CONNECT = 1,
    MQTT_PROT_CONNACK,
    MQTT_PROT_PUBLISH,
    MQTT_PROT_PUBACK,
    MQTT_PROT_PUBREC,
    MQTT_PROT_PUBREL,
    MQTT_PROT_PUBCOMP,
    MQTT_PROT_SUBSCRIBE,
    MQTT_PROT_SUBACK,
    MQTT_PROT_UNSUBSCRIBE,
    MQTT_PROT_UNSUBACK,
    MQTT_PROT_PINGREQ,
    MQTT_PROT_PINGRESP,
    MQTT_PROT_DISCONNECT
} mqtt_prot;

typedef struct {
    uint8_t qos;
    uint8_t topic_len;
    uint8_t *topic;
} mqtt_subs_params;

/**
 * @brief
 * Byte 1: Control Header.
 * Byte 2: Remaining length of Variable Header + Payload.
 * Bytes 3 and 4: 16bit protocol length.
 * Bytes 5 to 8: MQTT protocol name.
 * Byte 9: Protocol level, 0x04 for MQTT v3.1.1.
 * Byte 10: Connect flags
 *   Bit 0: Reserved
 *   Bit 1: Clean session flag. Set to 1 if is desirable to the server to 
 *   store previously values, 0 if desires to start new session.
 *   Bit 2: Will flag: Set to 1 if is desirable to to store a Will message
 *   in the server when Connect is accepted, 0 if not. Will message must be
 *   present in the payload.
 *   Bit 3 and 4: Will QoS flag: QoS level to be used when publishing will
 *   message. If Will Flag is set to 0, Will QoS must be 0. Otherwise,
 *   Will QoS accept values from 0 to 2.
 *   Bit 5: Will retain flag: Set to 1 if is desirable to retain Will message
 *   when published. Otherwise, 0. If Will flag is set to 0, Will retain must
 *   be 0.
 *   Bit 6: User name flag: Set to 1 if user name is present in the payload.
 *   Bit 7: Password flag: Set to 1 if password is present in the payload. If
 *   user name flag is 0, password must to be 0.
 * Bytes 11 and 12: 16bits keepalive interval.
 * The following bytes are destinated for ClientID, Will Topic, Will Message,
 * User Name and Password. Before each field, must preceed a two bytes field
 * length.
 * @param to_send Formated 'connect' protocol packet.
 * @param conn_flags 1 byte bit to bit array with connection flags.
 * @param keepalive maximum time interval that is permitted to elapse between
 * the point at which the Client finishes transmitting one Control Packet and
 * the point it starts sending the next.
 * @param clientID Client Identification
 * @param username Username to connect MQTT server.
 * @param password Password to connect MQTT server.
 * @return Size in bytes to send.
 */
int mqtt_prot_connect(uint8_t *to_send,
                        uint8_t conn_flags,
                        uint16_t keepalive,
                        const char *clientID,
                        const char *username,
                        const char *password);

/**
 * @brief Answer packet for connect request.
 * @param msg Connack packet received.
 * @param bytes_received Number of bytes received.
 * @return The error code retrieved from connack.
 */
int mqtt_prot_connack(const uint8_t *msg, int bytes_received);

/**
 * @brief
 * Byte 1: Control packet header and publish flags.
 *   Bit 0: Retain flag: Server must retain the message and its QoS to deliver
 *   when others subscribes.
 *   Bits 1 and 2: QoS: Set 0 if no QoS is desired. 0b01 case is needed at least
 *   one delivery and 0b10 if needed a maximum of one delivery.
 *   Bit 3: DUP flag: Set to 0 if this is the first attempt of publishing the
 *   message, otherwise 1.
 *   Bit 4 to 7: Control packet.
 * Byte 2: Remaining length of Variable Header + Payload.
 * Bytes 3 and 4: 16bit topic name length.
 * Bytes 5 to n: Topic name.
 * Bytes n+1 to n+2: 16 bit message length.
 * Bytes n+3 and following: Message to publish.
 * @param pub_flags Publish flags for the message being published.
 * @param topic Topic in what pub_msg will be published.
 * @param pub_msg Message to be published.
 * @param to_send Formated 'publish' protocol packet.
 * @return Size in bytes to send.
 */
int mqtt_prot_publish(uint8_t pub_flags,
                        const char *topic,
                        const char *pub_msg,
                        uint8_t *to_send);

/**
 * @brief Answer packet for publish request.
 * @param msg Puback packet received.
 * @param bytes_received Number of bytes received.
 * @return Return 0 in case of success otherwise -1.
 */
int mqtt_prot_puback(const uint8_t *msg, int bytes_received);

/* TODO:
void mqtt_pubrec();

void mqtt_pubrel();

void mqtt_pubcomp();
*/

/**
 * @brief
 * Byte 1: Control Header.
 * Byte 2: Remaining length of Variable Header + Payload.
 * Byte 3: Packet Identifier MSB.
 * Byte 4: Packet Identifier LSB.
 * Following bytes are destinated for 2 bytes topic size, n bytes topic and 
 * 1 byte QoS.
 * @param params mqtt_subscribe_params pointer, contains topic and QoS values.
 * @param nbParams params array size.
 * @param to_send Formated 'subscribe' protocol packet.
 * @return Number of bytes to send
 */
int mqtt_prot_subscribe(mqtt_subs_params *params,
                        uint8_t nbParams,
                        uint8_t *to_send);

/**
 * @brief Answer packet for subscribe request.
 * @param msg Suback packet received
 * @param bytes_received Number of bytes received
 * @return Return 0 in case of success otherwise -1.
 */
int mqtt_prot_suback(const uint8_t *msg, int bytes_received);

/**
 * @brief
 * @param params mqtt_subscribe_params pointer, contains topic and QoS values.
 * @param nbParams params array size.
 * @param to_send Formated 'unsubscribe' protocol packet.
 * @return Number of bytes to send.
 */
int mqtt_prot_unsubscribe(mqtt_subs_params *params,
                            int nbParams,
                            uint8_t *to_send);

/**
 * @brief Answer packet for unsubscribe request.
 * @param msg Unsuback packet received.
 * @param bytes_received Number of bytes received
 * @return Return 1 in case of success otherwise -1.
 */
int mqtt_prot_unsuback(const uint8_t *msg, int bytes_received);

/* TODO:
void mqtt_pingreq();

void mqtt_pingresp();
*/

/**
 * @brief
 * @param to_send Formated 'disconnect' protocol packet.
 * @return Number of bytes to send.
 */
int mqtt_prot_disconnect(uint8_t *to_send);

#endif /* _MQTT_PROT_H_ */
