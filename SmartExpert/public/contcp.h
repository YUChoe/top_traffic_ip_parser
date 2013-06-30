#include "sockopt.h"

int Con_tcp(char *addr, int port);

int Con_tcp(char *addr, int port)
{
	int Sock;
	struct sockaddr_in caddr;

	if((Sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		LogToFile("Con_tcp", "Socket", 0, 0);
		return -1;
	}

	Setsockopt(Sock);
	memset(&caddr, 0, sizeof(addr));
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	caddr.sin_addr.s_addr = inet_addr(addr);

	if(connect(Sock, (struct sockaddr *)&caddr, sizeof(caddr)) == -1)
	{
		LogToFile("Con_tcp", "Connect", 1, errno);
		return -1;
	}

	return Sock;
}
