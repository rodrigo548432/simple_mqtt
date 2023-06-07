/**
 * @file network.h
 * @brief Networking functions declaration.
 */

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "stdio.h"
#include "stdint.h"

#define ENABLE_TRACES
#include "trace.h"

#define IPV4_MAX_LEN 17
#define BUFFER_SIZE 128
#define RECV_TIMEOUT 10000

/**
 * @brief
 * @param hostname Address to DNS resolution.
 * @param addr Resolved address.
 * @return 0 if success or -1 if fail.
 */
int resolve_hostname(const char *hostname, char *addr);

/**
 * @brief
 * @param hostname Hostname to open socket.
 * @param port Port to open socket.
 * @return Socket handler or -1 if fail.
 */
int socket_create(const char *hostname, int port);

/**
 * @brief
 * @param sockfd Socket handler.
 * @param buffer Buffer to send.
 * @param buffer_lenght Buffer lenght to send.
 * @return 0 if success or -1 if fail.
 */
int socket_send(int sockfd, const uint8_t *buffer, int buffer_lenght);

/**
 * @brief
 * @param sockfd Socket handler.
 * @param buffer Buffer to receive.
 * @return Buffer length or -1 if fail.
 */
int socket_receive(int sockfd, uint8_t *buffer);

/**
 * @brief 
 * @param sockfd Socket handler.
 * @return None.
 */
void socket_close(int sockfd);

#endif /* _NETWORK_H_ */
