#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../header/dccp.h"
/*
	This probe will scan for any available DCCP connections and print out their corresponding CCID if found.
*/
int main()
{
	int sock_fd = socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	socklen_t res_len = 6;
	uint8_t ccids[6];

	//Attempts to get the current socket options.
	if (getsockopt(sock_fd, SOL_DCCP, DCCP_SOCKOPT_AVAILABLE_CCIDS, ccids, &res_len)) {
		perror("getsockopt(DCCP_SOCKOPT_AVAILABLE_CCIDS)");
		return -1;
	}

	printf("%d CCIDs available:", res_len);
	for (int i = 0; i < res_len; i++)
	{
		printf(" %d", ccids[i]);
	}
	printf("\n");
	return res_len;
}
