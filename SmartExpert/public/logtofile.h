#ifndef _NO_MUTEX_USE_
extern pthread_mutex_t Log_Mutex;
#endif

void LogToFile(char *Head, char *Body, int Error, int ErrCode)
{
	FILE *fp = NULL;
	time_t the_time, old_time;
	struct tm *tm_ptr = NULL;
	struct tm *oldtm_ptr = NULL;
	char oldname[255];
	static char logname[255];

	time(&the_time);
	tm_ptr = localtime(&the_time);
#ifndef _NO_MUTEX_USE_
	pthread_mutex_lock(&Log_Mutex);
#endif
	if(Head == NULL)
	{
		sprintf(logname, "%s%d-%d-%d.log", LOGFILE, tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday);
#ifndef _NO_MUTEX_USE_
		pthread_mutex_unlock(&Log_Mutex);
#endif
		return;
	}

	sprintf(logname, "%s%d-%d-%d.log", LOGFILE, tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday);

	if(access(logname, F_OK) == -1)
	{
		oldtm_ptr = (struct tm *)malloc(sizeof(struct tm));
		old_time = the_time - 86400 * 7;
		tm_ptr = localtime(&old_time);
		memcpy(oldtm_ptr, tm_ptr, sizeof(struct tm));
		sprintf(oldname, "%s%d-%d-%d.log", LOGFILE, oldtm_ptr->tm_year+1900, oldtm_ptr->tm_mon+1, oldtm_ptr->tm_mday);
		unlink(oldname);
		free(oldtm_ptr);
		tm_ptr = localtime(&the_time);
	}
	fp = fopen(logname, "a");
	fprintf(fp, "[%d-%02d-%02d %02d:%02d:%02d] ", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1,
	tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
	if(Error)
		fprintf(fp, "Error - %s : %s - ErrorCode : %d\n", Head, Body, ErrCode);
	else
		fprintf(fp, "Message - %s : %s\n", Head, Body);
	fclose(fp);
#ifndef _NO_MUTEX_USE_
	pthread_mutex_unlock(&Log_Mutex);
#endif
}

