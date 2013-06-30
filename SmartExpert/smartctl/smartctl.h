#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#define CFGFILE "../etc/tracetrplus.conf"
#define PROCNAME "tracetrplus"

#define Free(x) if(x != NULL){free(x), x = NULL;}

#include "../public/readconf.h"

#define MODE_STAT 0
#define MODE_START 1
#define MODE_STOP 2

int Startmodule(char *module, char *arg);
int Getpid(char *pidfile);
int Stopmodule(int pid, char *module);
int Checkproc(int pid, char *exepath);
int GetModeFromArg(int argc, char **argv);
void ShowStat();
void StartSmartExpert();
void StopSmartExpert();

int GetModeFromArg(int argc, char **argv)
{
	int Ret = -1;
	
	if(argc == 1)
		Ret =  MODE_STAT;
	else if(argc == 2)
	{
		if(!strcmp(argv[1], "start"))
			Ret = MODE_START;
		else if(!strcmp(argv[1], "stop"))
			Ret = MODE_STOP;
	}	
	
	return Ret;
}

void ShowStat()
{
	char ProcPidFile;
	int ProcPid;

	sprintf(ProcPidFile, ".%spid", PROCNAME);
	ProcPid = Getpid(ProcPidFile);

	if(Checkproc(ProcPid, ProcPidFile))
		printf("  SmartExpert : Running!\n");
	else
		printf("  SmartExpert : Stop!\n");
}

void StartSmartExpert()
{
	char Interval[255];
	char Device[255];
	char *
}

void StopSmartExpert()
{
}

void Usage(char *Proc)
{
}

int Checkproc(int pid, char *exepath)
{
    char tmp[32];
    char exec[255];
    int ret;

    sprintf(tmp, "/proc/%d/exe", pid);

    if((ret = readlink(tmp, exec, 255)) > 0)
    {
        exec[ret] = '\0';
        return !strcmp(exec, exepath);
    }
    return 0;
}

int Startmodule(char *module, char *arg)
{
    char exepath[255];
    int pid;

    sprintf(exepath, ".%spid", module);
    pid = Getpid(exepath);
    sprintf(exepath, "%s%s", BASEDIR, module);
    if(Checkproc(pid, exepath))
        return 0;
    sprintf(exepath, "./%s %s", module, arg);

    system(exepath);
}

int Getpid(char *pidfile)
{
    int fd;
    char buf[16];

    fd = open(pidfile, O_RDONLY);
    Readline(0, NULL, 0, 1);
    if(Readline(fd, buf, 16, 0) <= 0)
    {
        close(fd);
        return -1;
    }
    close(fd);
    return atoi(buf);
}

int Stopmodule(int pid, char *module)
{
    char path[255];

    sprintf(path, "%s%s", BASEDIR, module);

    if(Checkproc(pid, path))
    {
        kill(pid, SIGINT);
        printf("Module : %s Stop (PID : %d)\n", module, pid);
    }
    else
        printf("Module : %s Stop Error!\n", module);
}

