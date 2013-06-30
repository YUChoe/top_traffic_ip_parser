#include "tracetr.h"

int loop = 1;

void daemon_init(const char *pname, int facility);
void *thread_monitor(void *data);

int main(int argc, char **argv)
{
	pthread_t *p_thread = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *capdev;
	int i;
	HashTable TmpTraceData;
	struct _HASH_LIST_ *Cur = NULL;
	FILE *fp = NULL;
	char DateTime[20];
	char DataFile[28];
	int Partition = 0;
	int Interval = 5;

	int tmpfd;

	if(argc < 3)
	{
		printf("Usage : %s <Interval(min)> eth1 eth2 ...\n", argv[0]);
		exit(1);
	}
	
	Interval = atoi(argv[1]);
	printf("Set Interval : %d(min)\n", Interval);
	
	daemon_init(argv[0], 0);
	signal(SIGINT, Sighandle);
	Writepid(getpid(), PIDFILE);
	LogToFile(NULL, NULL, 0, 0);
	LogToFile("Trace Traffic Start Up", "=================================", 0, 0);
		
	p_thread = (pthread_t *)malloc(sizeof(pthread_t)*(argc-2));
	memset(&TraceData, 0, sizeof(HashTable));
	TraceData.Bucket = (struct _HASH_LIST_ **)malloc(sizeof(struct _HASH_LIST_ *) * HASHNUM);
	memset(TraceData.Bucket, 0, sizeof(struct _HASH_LIST_ *) * HASHNUM);

	memset(SvcPortByDynamic, 0, sizeof(SvcPortByDynamic));

	//tmpfd = open(".tracetrpid", O_RDONLY);
	
	for(i = 0; i < argc-2; i++)
	{
		if((capdev = GetPcapFromDevice(argv[i+2])) == NULL)
			exit(0);
		if(pcap_datalink(capdev) != DLT_EN10MB)
		{
			LogToFile("Wrong Device Interface", argv[i+2], 0, 0);
			exit(0);
		}

		pthread_mutex_lock(&Async_Mutex);
		pthread_create(&p_thread[i], NULL, thread_monitor, (void *)capdev);
		pthread_cond_wait(&Async_Cond, &Async_Mutex);
		pthread_mutex_unlock(&Async_Mutex);
	}

	if(sigsetjmp(jmp, 1))
		LogToFile("Trace Traffic Shutdown", "=================================", 0, 0);
	while(loop)
	{
		sleep(Interval * 60);
		pthread_mutex_lock(&TrafficData_Mutex);
		memcpy(&TmpTraceData, &TraceData, sizeof(HashTable));
		memset(&TraceData, 0, sizeof(HashTable));
		TraceData.Bucket = (struct _HASH_LIST_ **)malloc(sizeof(struct _HASH_LIST_ *) * HASHNUM);
		memset(TraceData.Bucket, 0, sizeof(struct _HASH_LIST_ *) * HASHNUM);
		pthread_mutex_unlock(&TrafficData_Mutex);

		GetDateTime(DateTime);
		GetDataFileName(DataFile, &Partition);
		fp = fopen(DataFile, "a");
		for(i = 0; i < HASHNUM; i++)
		{
			Cur = TmpTraceData.Bucket[i];
			while(Cur)
			{
				fprintf(fp, "%s,%s,%s,%d,%d,%d,%d,%d,%s\n", 
				DateTime, Cur->SIP, Cur->DIP, Cur->SPort, Cur->DPort, 
				Cur->PacketBytes, Cur->PacketCount, Cur->Protocol, Cur->Flow);
				Cur = Cur->Next;
			}
		}
		fprintf(fp, "\n");
		fclose(fp);
		DecidePartition(DataFile, &Partition);

		HashTableClear(TmpTraceData.Bucket);
		Free(TmpTraceData.Bucket);
	}

	for(i = 0; i < argc-2; i++)
		pthread_cancel(p_thread[i]);
	for(i = 0; i < argc-2; i++)
		pthread_join(p_thread[i], NULL);

	Free(p_thread);
	HashTableClear(TraceData.Bucket);
	Free(TraceData.Bucket);
}

void *thread_monitor(void *data)
{
    pcap_t *capdev;
    u_char *packet = NULL;
    u_char *Data = NULL;
    struct pcap_pkthdr pcap_h;
    struct ip *ip_hdr;
    struct ether_header *eth_hdr;
    struct tcphdr_c *tcp_hdr;
    struct udphdr *udp_hdr;
    unsigned short SPort, DPort;
	unsigned short Protocol;
    char SIP[16], DIP[16];
	int Flow;

	pthread_mutex_lock(&Async_Mutex);
    capdev = (pcap_t *)data;
    pthread_cond_signal(&Async_Cond);
    pthread_mutex_unlock(&Async_Mutex);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(loop)
    {
        if((packet = (u_char *)pcap_next(capdev, &pcap_h)) == NULL)
	{
		LogToFile("pcap_next return NULL", pcap_geterr(capdev), 0, 0);
		continue;
	}
	LogToFile("TestCode", "First Len :", 1, pcap_h.len);
        eth_hdr = (struct ether_header *)packet;

        pthread_mutex_lock(&TrafficData_Mutex);

		if(ntohs(eth_hdr->ether_type) == ETHERTYPE_IP)
        {
			ip_hdr = (struct ip *)(packet + sizeof(struct ether_header));
			
			strcpy(SIP, (char *)inet_ntoa(ip_hdr->ip_src));
			strcpy(DIP, (char *)inet_ntoa(ip_hdr->ip_dst));
			SPort = DPort = 0;

			Flow = ANYFLOW;
			Protocol = ip_hdr->ip_p;
			switch(ip_hdr->ip_p)
			{
				case IPPROTO_TCP:
					tcp_hdr = (struct tcphdr_c *)(packet+sizeof(struct ether_header)+(ip_hdr->ip_hl<<2));
					SPort = ntohs(tcp_hdr->th_sport);
					DPort = ntohs(tcp_hdr->th_dport);
					if(tcp_hdr->th_flags == 0x12) // SYN ACK
					{
						if(SPort > 1024)
						{
							if(++SvcPortByDynamic[SPort-1024] == 0)
								SvcPortByDynamic[SPort-1024]--;
						}
					}
					else if((tcp_hdr->th_flags == 0x11) || (tcp_hdr->th_flags & 0x04))
					{
						if(DPort > 1024)
						{
							if(SvcPortByDynamic[DPort-1024] > 0)
								SvcPortByDynamic[DPort-1024]--;
						}
					}
					if((SPort <= 1024) || (DPort <= 1024))
					{
						Flow = (SPort<DPort?SERVER_TO_CLIENT:CLIENT_TO_SERVER);
					}
					else
					{
						if(SvcPortByDynamic[DPort-1024])
							Flow = CLIENT_TO_SERVER;
						else if(SvcPortByDynamic[SPort-1024])
							Flow = SERVER_TO_CLIENT;
					}
					break;
				case IPPROTO_UDP:
					udp_hdr = (struct udphdr *)(packet + sizeof(struct ether_header)+(ip_hdr->ip_hl<<2));
					SPort = ntohs(udp_hdr->source);
					DPort = ntohs(udp_hdr->dest);
					if((SPort <= 1024) || (DPort <= 1024))
						Flow = (SPort<DPort?SERVER_TO_CLIENT:CLIENT_TO_SERVER);

					break;
				case IPPROTO_ICMP:
					break;
				default:
					Protocol = 0;
					break;
			}
			//printf("Len : %d\n", pcap_h.len);
			LogToFile("Test Code", "Len", 1, pcap_h.len);
			if(HashTableInsert(TraceData.Bucket, SIP, DIP, SPort, DPort, pcap_h.len, Protocol, Flow) == HASH_NEW_ADD)
				TraceData.TotalCount++;
		}
		pthread_mutex_unlock(&TrafficData_Mutex);
	}
}


void daemon_init(const char *pname, int facility)
{
	int i;
	pid_t pid;

	if((pid = fork()) != 0)
		exit(0);
	setsid();
	signal(SIGHUP, SIG_IGN);

	if((pid = fork()) != 0)
		exit(0);
	//daemon_proc = 1;
	//chdir("/");
	umask(0);
	for(i=0; i<MAXFD; i++)
	close(i);
	//openlog(pname, LOG_PID, facility);
}

