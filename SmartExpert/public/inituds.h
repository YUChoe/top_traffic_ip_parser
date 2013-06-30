#include "sockopt.h"

int Init_uds(char *Path)
{
	int Sock;
	struct sockaddr_un saddr;

	if(access(Path, F_OK) == 0)
		unlink(Path);
	if((Sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		LogToFile("Init_uds", "Socket", 0, 0);
		return -1;
	}

	Setsockopt(Sock);
	memset(&saddr, 0, sizeof(saddr));
	saddr.sun_family = AF_UNIX;
	strcpy(saddr.sun_path, Path);

	if(bind(Sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		LogToFile("Init_uds", "Bind", 1, errno);
		return -1;
	}
	if(listen(Sock, 5) == -1)
	{
		LogToFile("Init_uds", "Listen", 0, 0);
		return -1;
	}

	return Sock;
}

int InitListenUdpUds(char *UdsPath)
{
	int Sock;
	struct sockaddr_un saddr;

	if(access(UdsPath, F_OK) == 0)
		unlink(UdsPath);
	if((Sock = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		LogToFile("Init_UdpUds", "Socket", 0, 0);
		return -1;
	}

	Setsockopt(Sock);
	memset(&saddr, 0, sizeof(saddr));
	saddr.sun_family = AF_UNIX;
	strcpy(saddr.sun_path, UdsPath);

	if(bind(Sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		LogToFile("Init_Udpuds", "Bind", 1, errno);
		return -1;
	}

	return Sock;
}

int Accept_Uds(int lsock)
{
    int Ret, len;
    struct sockaddr_un addr;

    len = sizeof(addr);

    Ret = accept(lsock, (struct sockaddr *)&addr, &len);

    return Ret;
}

