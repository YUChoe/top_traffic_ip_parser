#include "sockopt.h"

int Con_uds(char *Path);

int Con_uds(char *Path)
{
    int Sock;
    struct sockaddr_un addr;

    if((Sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        LogToFile("Con_uds", "Socket", 0, 0);
        return -1;
    }

    //Setsockopt(Sock);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, Path);

    if(connect(Sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        LogToFile("Con_uds", "Connect", 1, errno);
        return -1;
    }

    return Sock;
}

