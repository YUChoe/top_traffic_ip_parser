#include "readline.h"

int Confparsing(char *item, char *get, char *cfg);
char *LineParsing(char *item, char *cfg);

int Confparsing(char *item, char *get, char *cfg)
{
    int fd, cnt = 0;
    char buf[255];

    fd = open(cfg, O_RDONLY);
    Readline(0, NULL, 0, 1);
    while(Readline(fd, buf, 255, 0) > 0)
    {
        if(!strncmp(buf, item, strnlen(item)))
        {
            strcpy(get, buf+strlen(item)+1);
            cnt = strlen(get);
            break;
        }
    }
    close(fd);
    return cnt;
}

char *LineParsing(char *item, char *cfg)
{
    char buf[255];
    char *ret = NULL;
    int fd, sflag = 0;
    int size = 0;

    fd = open(cfg, O_RDONLY);
    Readline(0, NULL, 0, 1); //함수의 스택틱 변수 초기화
    while(Readline(fd, buf, 255, 0) > 0)
    {
        if(sflag)
        {
            if(!strlen(buf))
                break;
            if(buf[0] == '#')
                continue;
            ret = (char *)realloc(ret, (size+strlen(buf)+1+1));
            memset(ret+size, 0, strlen(buf)+1);
            size += strlen(buf)+1;
            sprintf(ret, "%s%s\n", ret, buf);
        }
        if(!strncmp(buf, item, strlen(item)))
            sflag = 1;
    }
    close(fd);
    return ret;
}

