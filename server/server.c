#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "../header/dccp.h"

#define PORT 1337
#define RECORD_CODE 42

//Useful for graceful exiting from error
int error_exit(const char* str)
{
	perror(str);
	exit(errno);
}

/*
	Main function of client
*/
int main(int argc, char **argv)
{
	int listen_sock = socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	if (listen_sock < 0)
		error_exit("socket");

	struct sockaddr_in servaddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(PORT),
	};

	if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &(int) {
		       1}, sizeof(int)))
		error_exit("setsockopt(SO_REUSEADDR)");

	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)))
		error_exit("bind");
	
	//Sets the port and record code options required by DCCP
	if (setsockopt(listen_sock, SOL_DCCP, DCCP_SOCKOPT_RECORD, &(int) {
		       htonl(RECORD_CODE)}, sizeof(int)))
		error_exit("setsockopt(DCCP_SOCKOPT_RECORD)");

	if (listen(listen_sock, 1))
		error_exit("listen");
	//connection activity
	for (;;) {

		printf("Waiting for connection...\n");

		struct sockaddr_in client_addr;
		socklen_t addr_len = sizeof(client_addr);

		int conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
		if (conn_sock < 0) {
			perror("accept");
			continue;
		}

		printf("Connection received from %s:%d\n",
		       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		//Handles recieved datagram
		for (;;) {
			char datagramBuffer[1024];
			int result = recv(conn_sock, datagramBuffer, sizeof(datagramBuffer), 0);
			if (result > 0)
			{
				printf("Received: %s\n", datagramBuffer);
			}
			else
			{
				break;
			}

		}
		//ensures safe closure of application
		close(conn_sock);
	}	
}
