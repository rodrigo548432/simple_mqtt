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
	int sockfd;

	if (argc < 4) {
		printf("Not enough arguments\n");
		return -1;
	}
	const char *hostname = argv[1];
	int port = atoi(argv[2]);
	const char **topics = NULL;
	int topics_len = argc - 3;
	topics = (const char**)malloc(topics_len * sizeof(const char*));
	for (int i = 0; i < topics_len; i++)
		topics[i] = argv[i+3];

	printf("Start Simple MQTT packet sender:\n");
	printf(" Hostname: %s\n", hostname);
	printf(" Port: %d\n", port);
	printf(" Topics: ");
	for (int i = 0; i < topics_len; i++)
		printf("%s ", topics[i]);
	printf("\n");

	printf("Creating connection\n");
	sockfd = mqtt_connect_simple(hostname, port, CLIENTID);
	if (sockfd < 0) {
		printf("MQTT connect failure!\n");
		return -1;
	}

	subscribe_parameters *subs_params;
	subs_params = (subscribe_parameters*)malloc(topics_len * sizeof(subscribe_parameters));
	for (int i = 0; i < topics_len; i++) {
		subs_params[i].topic_len = strlen(topics[i]);
		subs_params[i].qos = SUBSCRIBE_QOS_2;
		subs_params[i].topic = topics[i];
	}
	printf("Subscribe to topics\n");
	if (mqtt_subscribe(sockfd, topics_len, subs_params) < 0) {
		printf("MQTT subscribe to topic failure!\n");
		goto finish;
	}

	for (int i = 0; i < topics_len; i++) {
		if (mqtt_publish(sockfd, PUBLISH_FLAG_QOS_2, subs_params[i].topic, "Teste 123") < 0) {
			printf("MQTT publish failure!\n");
			goto finish;
		}
		sleep(5);
	}

	mqtt_unsubscribe(sockfd, topics_len, subs_params);
	mqtt_disconnect(sockfd);

finish:
	for (int i = 0; i < topics_len; i++) {
		subs_params[i].topic = NULL;
		topics[i] = NULL;
	}
	free(subs_params);
	subs_params = NULL;
	free(topics);
	topics = NULL;
	return 0;
}
