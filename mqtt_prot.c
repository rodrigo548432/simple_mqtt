/**
 * @file mqtt_prot.c
 * @brief MQTT protocol functions implementation.
 */

#include "mqtt_prot.h"

int mqtt_prot_connect(uint8_t *to_send,
						uint8_t conn_flags,
						uint16_t keepalive,
						const char *clientID,
						const char *username,
						const char *password)
{
	uint8_t conn_pkt[MQTT_PROT_PACKET_LEN];
	uint16_t dummy;
	uint8_t i = 0;
	int bytes_to_send;

	print_dbg("IN");

	memset(&conn_pkt[0], 0, MQTT_PROT_PACKET_LEN * sizeof(char));

	/* FIXED HEADER */
	conn_pkt[i] = (MQTT_PROT_CONNECT << 4);
	conn_pkt[++i] = 0x00; // Start with 0 value as we don't have the final size.

	/* VARIABLE HEADER */
	conn_pkt[++i] = 0x00;
	conn_pkt[++i] = 0x04;
	conn_pkt[++i] = 'M';
	conn_pkt[++i] = 'Q';
	conn_pkt[++i] = 'T';
	conn_pkt[++i] = 'T';
	conn_pkt[++i] = 0x04;
	conn_pkt[++i] = conn_flags;

	/* TODO: Receive KeepAlive value. */
	conn_pkt[++i] = ((keepalive & 0xFF00) >> 8);
	conn_pkt[++i] = (keepalive & 0x00FF);

	/* PAYLOAD */
	dummy = strlen(clientID);
	conn_pkt[++i] = (dummy >> 8);
	conn_pkt[++i] = dummy;
	for (int j = 0; j < dummy; j++)
		conn_pkt[++i] = clientID[j];
	if (username != NULL) {
		dummy = strlen(username);
		for (int j = 0; j < dummy; j++)
			conn_pkt[++i] = username[j];
	}
	if (password != NULL) {
		dummy = strlen(password);
		for (int j = 0; j < dummy; j++)
			conn_pkt[++i] = password[j];
	}

	conn_pkt[1] = (++i - 2); // Set size of Byte 2 as VARIABLE HEADER + PAYLOAD.

	bytes_to_send = i;
	memcpy(&to_send[0], conn_pkt, bytes_to_send * sizeof(uint8_t));

	return bytes_to_send;
}

int mqtt_prot_connack(const uint8_t *msg, int bytes_received)
{
	print_dbg("IN");

	if (msg == NULL || bytes_received != 4 ||
		msg[0] != (MQTT_PROT_CONNACK << 4))
		return -1;

	return (int)msg[3];
}

int mqtt_prot_disconnect(uint8_t *to_send)
{
	uint8_t disc_pkt[2];
	int bytes_to_send;

	print_dbg("IN");

	memset(&disc_pkt[0], 0, 2 * sizeof(uint8_t));
	disc_pkt[0] = (MQTT_PROT_DISCONNECT << 4);

	bytes_to_send = strlen(disc_pkt);
	memcpy(&to_send[0], disc_pkt, bytes_to_send * sizeof(uint8_t));

	return bytes_to_send;
}

int mqtt_prot_subscribe(mqtt_subs_params *params,
						uint8_t nbParams,
						uint8_t *to_send)
{
	uint8_t subs_pkt[MQTT_PROT_PACKET_LEN], i = 0;
	uint16_t topic_len;
	int bytes_to_send;

	print_dbg("IN");

	memset(&subs_pkt[0], 0, MQTT_PROT_PACKET_LEN * sizeof(uint8_t));

	/* FIXED HEADER */
	subs_pkt[i] = (MQTT_PROT_SUBSCRIBE << 4);
	subs_pkt[++i] = 0x00;

	/* VARIABLE HEADER */
	subs_pkt[++i] = 0x00;
	subs_pkt[++i] = 0x02;

	/* PAYLOAD */
	for (int j = 0; j < nbParams; j++) {
		subs_pkt[++i] = (uint8_t)(params[j].topic_len >> 8);
		subs_pkt[++i] = (uint8_t)params[j].topic_len;
		for (int k = 0; k < params[j].topic_len; k++) {
			subs_pkt[++i] = params[j].topic[k];
		}
		subs_pkt[++i] = params->qos;
	}

	subs_pkt[1] = (++i - 2);

	bytes_to_send = i;
	memcpy(&to_send[0], subs_pkt, bytes_to_send * sizeof(uint8_t));

	return bytes_to_send;
}

int mqtt_prot_suback(const uint8_t *msg, int bytes_received)
{
	print_dbg("IN");

	if (msg == NULL || bytes_received < 5 ||
		msg[0] != (MQTT_PROT_SUBACK << 4))
		return -1;

	for (int i = 4; i < bytes_received; i++) {
		if (msg[i] == 0x80) {
			print_err("Topic %d bad QoS", i - 3);
			return -1;
		}
	}

	return 0;
}

int mqtt_prot_unsubscribe(mqtt_subs_params *params,
							int nbParams,
							uint8_t *to_send)
{
	uint8_t unsub_pkt[MQTT_PROT_PACKET_LEN];
	uint16_t topic_len, i = 0;
	int bytes_to_send;

	print_dbg("IN");

	memset(&unsub_pkt[0], 0, MQTT_PROT_PACKET_LEN * sizeof(uint8_t));

	unsub_pkt[i] = (MQTT_PROT_UNSUBSCRIBE << 4) | (1 << 1);
	unsub_pkt[++i] = 0x00;

	unsub_pkt[++i] = 0x00;
	unsub_pkt[++i] = 0x02;

	for (int j = 0; j < nbParams; j++) {
		unsub_pkt[++i] = (uint8_t)(params[j].topic_len >> 8);
		unsub_pkt[++i] = (uint8_t)params[j].topic_len;
		for (int k = 0; k < params[j].topic_len; k++)
			unsub_pkt[++i] = params[j].topic[k];
	}

	unsub_pkt[1] = (++i - 2);

	bytes_to_send = i;
	memcpy(&to_send[0], unsub_pkt, bytes_to_send * sizeof(uint8_t));

	return bytes_to_send;
}

int mqtt_prot_unsuback(const uint8_t *msg, int bytes_received)
{
	if (msg == NULL || bytes_received < 4 ||
		msg[0] != (MQTT_PROT_UNSUBACK << 4))
		return -1;

	return 0;
}

int mqtt_prot_publish(uint8_t pub_flags,
						const char *topic,
						const char *pub_msg,
						uint8_t *to_send)
{
	uint8_t pub_pkt[MQTT_PROT_PACKET_LEN];
	uint16_t topic_len, pub_msg_len, i = 0;
	int bytes_to_send;

	print_dbg("IN");

	memset(&pub_pkt[0], 0, MQTT_PROT_PACKET_LEN * sizeof(uint8_t));
	topic_len = (uint8_t)strlen(topic);
	pub_msg_len = (uint8_t)strlen(pub_msg);

	pub_pkt[i] = (MQTT_PROT_PUBLISH << 4) | (pub_flags & 0xF);
	pub_pkt[++i] = 0x00;

	pub_pkt[++i] = (uint8_t)(topic_len >> 8);
	pub_pkt[++i] = (uint8_t)topic_len;
	for (int j = 0; j < topic_len; j++)
		pub_pkt[++i] = topic[j];

	pub_pkt[++i] = 0x00;
	pub_pkt[++i] = 0x02;

	pub_pkt[++i] = (uint8_t)(pub_msg_len >> 8);
	pub_pkt[++i] = (uint8_t)pub_msg_len;
	for (int j = 0; j < pub_msg_len; j++)
		pub_pkt[++i] = pub_msg[j];

	pub_pkt[1] = (++i - 2);

	bytes_to_send = i;
	memcpy(&to_send[0], pub_pkt, bytes_to_send * sizeof(uint8_t));

	return bytes_to_send;
}

int mqtt_prot_puback(const uint8_t *msg, int bytes_received)
{
	print_dbg("IN");

	if (msg == NULL || bytes_received < 4 || 
		msg[0] != (MQTT_PROT_PUBACK << 4))
		return -1;

	return 0;
}