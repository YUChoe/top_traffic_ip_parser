#ifndef _SOCKOPT_
#define _SOCKOPT_
void Setsockopt(int Sock);

void Setsockopt(int Sock)
{
    struct timeval tv;
    char tmp;
	int time = 1;

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    //setsockopt(Sock, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp, sizeof(tmp));
    //setsockopt(Sock, SOL_SOCKET, SO_REUSEPORT, (char *)&tmp, sizeof(tmp));
	setsockopt(Sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	//if(setsockopt(Sock, IPPROTO_TCP, TCP_MAXRT, &time, sizeof(time)) == -1)
	//	printf("sockopt : %d\n", errno);
    setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); 

}
#endif
