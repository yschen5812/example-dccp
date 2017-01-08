#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "../header/dccp.h"

int error_exit(const char *str)
{
	perror(str);
	exit(errno);
}

int main(int argc, char *argv[])
{
	if (argc < 5) {
		printf("Usage: ./client <server address> <port> <record code> <message 1> [message 2] ... \n");
		exit(-1);
	}
	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
	};
	//Checks to ensure that the user has entered a valid address
	if (!inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr)) {
		printf("Invalid address %s\n", argv[1]);
		exit(-1);
	}

	int socket_fd = socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	if (socket_fd < 0)
	{
		error_exit("socket");
	}
	//Attempts to set the socket options
	if (setsockopt(socket_fd, SOL_DCCP, DCCP_SOCKOPT_RECORD, &(int) {htonl(atoi(argv[3]))}, sizeof(int)))
	{
		error_exit("setsockopt(DCCP_SOCKOPT_RECORD)");
	}
	//Attempts to connect to a socket
	if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)))
	{
		error_exit("connect");
	}

	uint32_t mps;
	//Sets res_len to maximum packet size
	socklen_t res_len = sizeof(mps);
	if (getsockopt(socket_fd, SOL_DCCP, DCCP_SOCKOPT_GET_CUR_MPS, &mps, &res_len))
		error_exit("getsockopt(DCCP_SOCKOPT_GET_CUR_MPS)");
	printf("Maximum Packet Size: %d\n", mps);

	for (int i = 4; i < argc; i++) {
		if (send(socket_fd, argv[i], strlen(argv[i]) + 1, 0) < 0)
		{
			error_exit("send");
		}
	}
	usleep(5 * 1000);
	//ensures termination of activity
	close(socket_fd);
	return 0;
}
