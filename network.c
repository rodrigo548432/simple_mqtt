/**
 * @file network.c
 * @brief Networking functions implementation.
 */

#include "stdlib.h"
#include "sys/socket.h"
#include "netdb.h"
#include "arpa/inet.h"
#include "string.h"
#include "unistd.h"

#include "network.h"

int resolve_hostname(const char *hostname, char *addr)
{
	int addr_len, i;
	struct hostent *he;
	struct in_addr **addr_list;

	if (hostname == NULL) {
		print_err("Hostname is NULL");
		return -1;
	}

	he = (struct hostent *)gethostbyname(hostname);
	if (he == NULL) {
		print_err("couldn't resolve %s", hostname);
		return -1;
	}

	addr_list = (struct in_addr **)he->h_addr_list;
	for (i = 0; addr_list[i] != NULL; i++) {
		strcpy(addr, inet_ntoa(*addr_list[i]));
		break;
	}

	print_dbg("%s resolved as %s", hostname, addr);

	return 0;
}

int socket_create(const char *hostname, int port)
{
	int ret = 0, sock = 0;
	struct sockaddr_in serv_addr;
	char *addr;

	addr = (char *)malloc(IPV4_MAX_LEN * sizeof(char));
	memset(addr, 0, IPV4_MAX_LEN * sizeof(char));

	if (resolve_hostname(hostname, addr) < 0)
		goto fail;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		goto fail;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_pton(AF_INET, (const char *)addr, &serv_addr.sin_addr);

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		goto fail;

	return sock;
fail:
	free(addr);
	addr = NULL;
	return -1;
}

int socket_receive(int sockfd, uint8_t *buffer)
{
	ssize_t bytes_recv;
	uint8_t received[BUFFER_SIZE];

	memset(&received[0], 0, BUFFER_SIZE * sizeof(uint8_t));

	bytes_recv = recv(sockfd, received, BUFFER_SIZE, 0);
	if (bytes_recv < 0)
		return -1;
	
#if 0
	int dummy = 0;
	printf("recv buffer:\n");
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("0x%x ", received[dummy]);
			dummy++;
		}
		printf("\n");
	}
#endif

	memcpy(buffer, received, BUFFER_SIZE * sizeof(uint8_t));
	return (int)bytes_recv;
}

int socket_send(int sockfd, const uint8_t *buffer, int buffer_lenght)
{
	uint8_t *to_send;

	if (buffer == NULL) {
		print_err("Buffer is NULL");
		return -1;
	}

	to_send = (uint8_t *)malloc(buffer_lenght * sizeof(uint8_t));
	memcpy(&to_send[0], buffer, buffer_lenght * sizeof(uint8_t));

	if (send(sockfd, to_send, buffer_lenght, 0) < 0)
		return -1;

	free(to_send);
	to_send = NULL;

	return 0;
}

void socket_close(int sockfd)
{
	close(sockfd);
}
