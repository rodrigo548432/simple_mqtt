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
	int port = atoi(argv[2]);
	char **topic = NULL;
	int topics_len = argc - 3;
	topic = (char**)malloc(topics_len * sizeof(char*));
	for (int i = 0; i < topics_len; i++) {
		int topic_str_len = strlen(argv[i+3]);
		topic[i] = (char*)malloc(topic_str_len * sizeof(char));
		strncpy(topic[i], argv[i+3], topic_str_len * sizeof(char));
	}

	printf("Start MQTT packet sender:\n");
	printf(" Hostname: %s\n", hostname);
	printf(" Port: %d\n", port);
	printf(" Topics: ");
	for (int i = 0; i < topics_len; i++)
		printf("%s ", topic[i]);
	printf("\n");

	printf("Starting Simple MQTT\n");
	sockfd = mqtt_connect_simple(hostname, port, CLIENTID);
	if (sockfd < 0) {
		printf("MQTT connect failure!\n");
		return -1;
	}

	subscribe_parameters *subs_params;
	subs_params = (subscribe_parameters*)malloc(topics_len * sizeof(subscribe_parameters));
	for (int i = 0; i < topics_len; i++) {
		subs_params[i].topic_len = strlen(topic[i]);
		subs_params[i].qos = 2;
		subs_params[i].topic = (char*)malloc(subs_params[i].topic_len * sizeof(char));
		strncpy(subs_params[i].topic, topic[i], subs_params[i].topic_len * sizeof(char));
	}
	printf("Subscribe to topic\n");
	if (mqtt_subscribe(sockfd, topics_len, subs_params) < 0) {
		printf("MQTT subscribe to topic failure!\n");
		goto finish;
	}

	for (int i = 0; i < topics_len; i++) {
		if (mqtt_publish(sockfd, subs_params[i].topic, "Teste 123") < 0) {
			printf("MQTT publish failure!\n");
			goto finish;
		}
		sleep(5);
	}

	mqtt_unsubscribe(sockfd, topics_len, subs_params);
	mqtt_disconnect(sockfd);

finish:
	for (int i = 0; i < topics_len; i++) {
		free(subs_params[i].topic);
		subs_params[i].topic = NULL;
	}
	free(subs_params);
	subs_params = NULL;
	return 0;
}
