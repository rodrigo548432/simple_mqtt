/**
 * @file main.c
 * @brief MQTT application testing.
 *
*/

#include "stdio.h"
#include "stdlib.h"
#include "mqtt.h"
#include "unistd.h"

#define CLIENTID "BIRINGA"

int main(int argc, char *argv[])
{
	int ret = 0, hn_len = 0, sockfd, buffer_len;

	if (argc < 4) {
		printf("Not enough arguments\n");
		return -1;
	}
	const char *hostname = argv[1];
	const char *topic = argv[3];
	int port = atoi(argv[2]);

	printf("Received params:\n");
	printf("Hostname: %s\n", hostname);
	printf("Port: %d\n", port);
	printf("Topic: %s\n", topic);

	printf("Starting Simple MQTT\n");
	sockfd = mqtt_connect_simple(hostname, port, CLIENTID);
	if (sockfd < 0) {
		printf("MQTT connect failure!\n");
		return -1;
	}

	subscribe_parameters subs_params[1];
	int topic_len = strlen(topic);
	subs_params[0].qos = 2;
	subs_params[0].topic = (char*)malloc(topic_len * sizeof(char));
	strncpy(subs_params[0].topic, topic, topic_len * sizeof(char));
	printf("Subscribe to topic\n");
	if (mqtt_subscribe(sockfd, 1, subs_params) < 0) {
		printf("MQTT subscribe to topic failure!\n");
		return -1;
	}

	while (1) {
		if (mqtt_publish(sockfd, subs_params[0].topic, "Teste 123") < 0) {
			printf("MQTT publish failure!\n");
			return -1;
		}
		sleep(5);

		break;
	}

	mqtt_unsubscribe(sockfd, 1, subs_params);
	mqtt_disconnect(sockfd);

	free(subs_params[0].topic);
	subs_params[0].topic = NULL;
	// free(subs_params);
	// subs_params = NULL;
	return 0;
}
