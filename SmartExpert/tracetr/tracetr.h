#include <pthread.h>
#include <errno.h>
#include <pcap.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define LOGFILE "../logs/tracetr"
#define PIDFILE ".tracetrpid"
#define CFGFILE "../etc/tracetr.conf"
#define MAXFD 64

#define HASHNUM 50
#define HASH_NEW_ADD 1

#define CLIENT_TO_SERVER 0 //"C-S"
#define SERVER_TO_CLIENT 1 //"S-C"
#define ANYFLOW 2 //"ANY"

#define Free(x) if(x != NULL){free(x); x = NULL;}

struct _HASH_LIST_
{
	char SIP[16];
	char DIP[16];
	unsigned short SPort;
	unsigned short DPort;
	unsigned int PacketBytes;
	unsigned int PacketCount;
	unsigned short Protocol;
	char Flow[4];
	struct _HASH_LIST_ *Next;
};

typedef struct _HASH_TABLE_
{
	unsigned int TotalCount; //아이템의 수
	struct _HASH_LIST_ **Bucket;
}HashTable;

unsigned int SvcPortByDynamic[65536-1024];
HashTable TraceData;

pthread_mutex_t Log_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Async_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Async_Cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t TrafficData_Mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef MEMWATCH
#include "../public/memwatch.h"
#endif

#include "../public/logtofile.h"
#include "../public/public.h"
#include "tcpheader.h"

unsigned int Hashing(const char *Item);
pcap_t *GetPcapFromDevice(char *DevName);
int HashTableInsert(struct _HASH_LIST_ **Bucket, char *SIP, char *DIP, unsigned short SPort, unsigned short DPort, unsigned int PacketLen, unsigned short Protocol, int Flow);
struct _HASH_LIST_ *HashTableFind(struct _HASH_LIST_ *Bucket, char *SIP, char *DIP, unsigned short SPort, unsigned short DPort);
void HashTableClear(struct _HASH_LIST_ **Bucket);
void GetDateTime(char *DateTime);
void GetDataFileName(char *FileName, int *Partition);
void DecidePartition(char *FileName, int *Partition);

void DecidePartition(char *FileName, int *Partition)
{
	int fd;
	struct stat st;

	if((fd = open(FileName, O_RDONLY)) <= 0)
		return;
	if(fstat(fd, &st))
	{
		close (fd);
		return;
	}
	if(st.st_size >= 1073741824) //1073741824(1GB)
		(*Partition)++;
		
	close(fd);
}

void GetDataFileName(char *FileName, int *Partition)
{
	time_t the_time;
	struct tm *tm_ptr = NULL;
	static struct tm oldtm = {0};

	time(&the_time);
	tm_ptr = localtime(&the_time);

	if(oldtm.tm_mday != 0)
	{
		if(tm_ptr->tm_mday != oldtm.tm_mday)
			(*Partition) = 0;
	}

	if(*Partition == 0)
		sprintf(FileName, "../data/%d-%02d-%02d.dat", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday);
	else
		sprintf(FileName, "../data/%d-%02d-%02d-%d.dat", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday, *Partition);

	memcpy(&oldtm, tm_ptr, sizeof(struct tm));

}

void GetDateTime(char *DateTime)
{
	time_t the_time;
	struct tm *tm_ptr = NULL;

	time(&the_time);
	tm_ptr = localtime(&the_time);

	sprintf(DateTime, "%d-%02d-%02d %02d:%02d:%02d", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1,
	    tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
}

void HashTableClear(struct _HASH_LIST_ **Bucket)
{
    int i;
    struct _HASH_LIST_ *Del;

    for(i = 0; i < HASHNUM; i++)
    {
        while(Bucket[i])
        {
            Del = Bucket[i];
            Bucket[i] = Del->Next;
            free(Del);
        }
    }
}

struct _HASH_LIST_ *HashTableFind(struct _HASH_LIST_ *Bucket, char *SIP, char *DIP, unsigned short SPort, unsigned short DPort)
{
    while(Bucket)
    {
        if((!strcmp(Bucket->SIP, SIP)) && (!strcmp(Bucket->DIP, DIP)) && (Bucket->SPort == SPort) && (Bucket->DPort == DPort))
            return Bucket;
        Bucket = Bucket->Next;
    }
    return NULL;
}

int HashTableInsert(struct _HASH_LIST_ **Bucket, char *SIP, char *DIP, unsigned short SPort, unsigned short DPort, unsigned int PacketLen, unsigned short Protocol, int Flow)
{
    unsigned int HashVal;
    struct _HASH_LIST_ *Cur;
	char Item[32];

	sprintf(Item, "%s%s", SIP, DIP);
    HashVal = Hashing(Item);

    ///TEST///
    if(HashVal >= HASHNUM)
        printf("Hashed Value : %d Hash Max : %d \n", HashVal, HASHNUM);
    ////////////

    if(Cur = HashTableFind(Bucket[HashVal], SIP, DIP, SPort, DPort))
    {
		Cur->PacketBytes += PacketLen;
        Cur->PacketCount++;
        //printf("Increase exist item\n");
        return 0;
    }
    else
    {
        //printf("Add new item\n");
        Cur = (struct _HASH_LIST_ *)malloc(sizeof(struct _HASH_LIST_));
		memset(Cur, 0, sizeof(struct _HASH_LIST_));
		strcpy(Cur->SIP, SIP);
		strcpy(Cur->DIP, DIP);
		Cur->SPort = SPort;
		Cur->DPort = DPort;
		Cur->PacketBytes = PacketLen;
		Cur->Protocol = Protocol;
		switch(Flow)
		{
			case CLIENT_TO_SERVER:
				strcpy(Cur->Flow, "C-S");
				break;
			case SERVER_TO_CLIENT:
				strcpy(Cur->Flow, "S-C");
				break;
			case ANYFLOW:
				strcpy(Cur->Flow, "ANY");
				break;
		}
		Cur->PacketCount = 1;
        Cur->Next = Bucket[HashVal];
        Bucket[HashVal] = Cur;
        return HASH_NEW_ADD;
    }
}

pcap_t *GetPcapFromDevice(char *DevName)
{
    pcap_t *Ret = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];

    if((Ret = pcap_open_live(DevName, BUFSIZ, 1, -1, errbuf)) == NULL)
        LogToFile("GetPcapFromDevice", errbuf, 1, 0);
    return Ret;
}


unsigned int Hashing(const char *Item)
{
    int HashedVal = 0;
    int tmp;

    while(*Item != '\0')
    {
        HashedVal = (HashedVal << 4) + (*Item);
        if(tmp = (HashedVal & 0xf0000000))
        {
            HashedVal = HashedVal ^ (tmp >> 24);
            HashedVal = HashedVal ^ tmp;
        }
        Item++;
    }
    if(HashedVal < 0)
        HashedVal *= -1;

    return (unsigned int)HashedVal%HASHNUM;
}

