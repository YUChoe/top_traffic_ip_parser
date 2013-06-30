#include "sockopt.h"

int Init_tcp(char *Port);

int Init_tcp(char *Port)
{
    int Sock;
	//char tmp;
	int optval = 1;
	struct timeval tv;
    struct sockaddr_in saddr;

	tv.tv_sec = 2;
	tv.tv_usec = 0;


    if((Sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        LogToFile("Init_tcp", "Socket", 0, 0);
        return -1;
    }

    //Setsockopt(Sock);
	setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(atoi(Port));

    if(bind(Sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
        LogToFile("Init_tcp", "Bind", 1, errno);
        return -1;
    }

    if(listen(Sock, 1024) == -1)
    {
        LogToFile("Init_tcp", "Listen", 0, 0);
        return -1;
    }

    return Sock;
}

