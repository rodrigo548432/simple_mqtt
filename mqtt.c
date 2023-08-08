/**
 * @file mqtt.c
 * @brief Main MQTT functions implementation.
 */

#include "mqtt.h"
#include "mqtt_prot.h"
#include "network.h"
#include "unistd.h"

#if 0
static const char *connack2str(mqtt_connack_err_codes err)
{
	switch (err) {
		case MQTT_CONNACK_ACCEPTED:
			return "MQTT_CONNACK_ACCEPTED";
		case MQTT_CONNACK_REFUSED_BAD_PROTOCOL:
			return "MQTT_CONNACK_REFUSED_BAD_PROTOCOL";
		case MQTT_CONNACK_REFUSED_ID_REJECTED:
			return "MQTT_CONNACK_REFUSED_ID_REJECTED";
		case MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE:
			return "MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE";
		case MQTT_CONNACK_REFUSED_BAD_USER_PASSWORD:
			return "MQTT_CONNACK_REFUSED_BAD_USER_PASSWORD";
		case MQTT_CONNACK_REFUSED_NOT_AUTHORIZED:
			return "MQTT_CONNACK_REFUSED_NOT_AUTHORIZED";
	}
}
#endif

static int valid_clientID(const char *clientID)
{
	int id_len = strlen(clientID);
	for (int i = 0; i < id_len - 1; i++) {
		if ((clientID[i] >= '0' && clientID[i] <= '9') ||
			(clientID[i] >= 'A' && clientID[i] <= 'Z') ||
			(clientID[i] >= 'a' && clientID[i] <= 'z')) {
			continue;
		} else {
			return -1;
		}
	}
	return 0;
}

int mqtt_connect(const char *hostname,
					int port,
					const char *clientID,
					mqtt_connect_flags connection_flags,
					int keepalive,
					const char *username,
					const char *password)
{
	int mqtt_socket, buf_len;
	uint8_t buffer[BUFFER_SIZE];
	uint8_t connect_flags;

	print_dbg("IN");

	if (hostname == NULL) {
		print_err("Hostname is NULL !!!");
		return -1;
	}
	if (clientID == NULL) {
		print_err("ClientID is mandatory !!!");
		return -1;
	}
	if (valid_clientID(clientID) == -1) {
		print_err("Invalid ClientID !!!");
		print_err("ClientID must contain [0-9][a-z][A-Z] only!");
		return -1;
	}

	mqtt_socket = socket_create(hostname, port);
	if (mqtt_socket < 0) {
		print_err("Couldn't create socket, MQTT not connecting ...");
		return -1;
	}

	print_dbg("Socket creation OK, try sending MQTT Connect");
	memset(&buffer[0], 0, BUFFER_SIZE * sizeof(uint8_t));

	if ((connect_flags & CONNECT_FLAG_USERNAME && username == NULL) ||
		(!(connect_flags & CONNECT_FLAG_USERNAME) && username != NULL) ||
		(connect_flags & CONNECT_FLAG_PASSWORD && password == NULL) ||
		(!(connect_flags & CONNECT_FLAG_PASSWORD) && password != NULL)) {
		print_wrn("Username/Passwrd connection flag is set but username field \
					is empty, we'll try to connect without authentication!");
		connect_flags &= !(CONNECT_FLAG_USERNAME);
		connect_flags &= !(CONNECT_FLAG_PASSWORD);
		username = NULL;
		password = NULL;
	}

	buf_len = mqtt_prot_connect(buffer, connect_flags, keepalive,
									clientID, username, password);
	if (socket_send(mqtt_socket, buffer, buf_len) < 0) {
		print_err("Couldn't send connect packet");
		return -1;
	}

	usleep(RECV_TIMEOUT);
	buf_len = socket_receive(mqtt_socket, buffer);
	if (buf_len < 0) {
		print_err("No answer packet");
		return -1;
	}

	if (mqtt_prot_connack(buffer, buf_len) != MQTT_CONNACK_ACCEPTED) {
		print_err("Bad coonack!");
		return -1;
	}

	return mqtt_socket;
}

int mqtt_connect_simple(const char *hostname,
							int port,
							const char *clientID)
{
	return mqtt_connect(hostname, port, clientID, CONNECT_FLAG_CLEAN_SESSION,
						60, NULL, NULL);
}

int mqtt_subscribe(int mqtt_socket,
					int subs_params_len,
					subscribe_parameters *subs_parameters)
{
	int buf_len;
	uint8_t buffer[BUFFER_SIZE];
	mqtt_subs_params *subs_params = (mqtt_subs_params*)subs_parameters;

	print_dbg("IN");

	if (subs_params == NULL || subs_params->topic == NULL) {
		print_err("Subscribe parameters is NULL !!!");
		goto fail;
	}

	memset(&buffer[0], 0, BUFFER_SIZE * sizeof(uint8_t));
	buf_len = mqtt_prot_subscribe(subs_params, subs_params_len, buffer);
	if (socket_send(mqtt_socket, buffer, buf_len) < 0) {
		print_err("Couldn't send subscribe packet");
		goto fail;
	}

	usleep(RECV_TIMEOUT);
	buf_len = socket_receive(mqtt_socket, buffer);
	if (buf_len < 0) {
		print_err("No answer packet");
		goto fail;
	}

	if (mqtt_prot_suback(buffer, buf_len) != 0) {
		print_err("Bad suback!");
		goto fail;
	}

	return 0;
fail:
	return -1;
}

int mqtt_publish(int mqtt_socket, mqtt_publish_flags publish_flags, 
					const char *topic, const char *msg)
{
	int buf_len;
	uint8_t buffer[BUFFER_SIZE];

	print_dbg("IN");

	if (topic == NULL || msg == NULL) {
		print_err("Topic or msg is NULL !!!");
		return -1;
	}

	/* TODO: Receive publish flags */
	memset(&buffer[0], 0, BUFFER_SIZE * sizeof(uint8_t));
	buf_len = mqtt_prot_publish(publish_flags, topic, msg, buffer);
	if (socket_send(mqtt_socket, buffer, buf_len) < 0) {
		print_err("Couldn't send publish packet");
		return -1;
	}

	usleep(RECV_TIMEOUT);
	buf_len = socket_receive(mqtt_socket, buffer);
	if (buf_len < 0) {
		print_err("No answer packet");
		return -1;
	}

	if (mqtt_prot_puback(buffer, buf_len) != 0) {
		print_err("Bad puback!");
		return -1;
	}

	return 0;
}

void mqtt_disconnect(int mqtt_socket)
{
	int buf_len;
	uint8_t buffer[BUFFER_SIZE];

	print_dbg("IN");

	memset(&buffer[0], 0, BUFFER_SIZE * sizeof(uint8_t));
	buf_len = mqtt_prot_disconnect(buffer);
	if (socket_send(mqtt_socket, buffer, buf_len) < 0)
		print_wrn("Couldn't send disconnect packet");
	
	socket_close(mqtt_socket);
}

int mqtt_unsubscribe(int mqtt_socket,
						int subs_params_len,
						subscribe_parameters *subs_parameters)
{
	int buf_len;
	uint8_t buffer[BUFFER_SIZE];
	mqtt_subs_params *subs_params = (mqtt_subs_params*)subs_parameters;

	print_dbg("IN");

	if (subs_params == NULL || subs_params->topic == NULL) {
		print_err("Topic is NULL !!!");
		goto fail;
	}

	memset(&buffer[0], 0, BUFFER_SIZE * sizeof(uint8_t));
	buf_len = mqtt_prot_unsubscribe(subs_params, subs_params_len, buffer);
	if (socket_send(mqtt_socket, buffer, buf_len) < 0) {
		print_err("Couldn't send unsubscribe packet");
		goto fail;
	}

	usleep(RECV_TIMEOUT);
	buf_len = socket_receive(mqtt_socket, buffer);
	if (buf_len < 0) {
		print_err("No answer packet");
		goto fail;
	}

	if (mqtt_prot_unsuback(buffer, buf_len) != 0) {
		print_err("Bad unsuuback!");
		goto fail;
	}

	return 0;
fail:
	return -1;
}