char *Receive(int Sock, unsigned int *rsize);
int SendWhile(int Sock, char *SendBuf, size_t BufSize);

int SendWhile(int Sock, char *SendBuf, size_t BufSize)
{
    size_t CurSendSize = 0;
    size_t TmpSize = 0;

    while(CurSendSize < BufSize)
    {
        if((TmpSize = send(Sock, SendBuf+CurSendSize, BufSize-CurSendSize, 0)) == -1)
            return 0;

        CurSendSize += TmpSize;
    }

    return 1;
}

char *Receive(int Sock, unsigned int *rsize)
{
    char *RecBuf = NULL;
    char *Tmp = NULL;
    char buf[1460];
    short len, OutCnt = 0; // short is -32768~32767
    unsigned int size = 32;

    *rsize = 0;
    do
    {
        len = read(Sock, buf, sizeof(buf));
        if(len > 0)
        {
            *rsize += len;
            OutCnt = 0;
            Tmp = (char *)realloc(RecBuf, size + len);
            if(Tmp != NULL)
            {
                //memset(Tmp+(size-32), 0, len);
                memset(Tmp+(size-32), 0, len+32);
                RecBuf = Tmp;
                memcpy(RecBuf+(size-32), buf, len);
                //strncat(RecBuf, buf, len);
                size += len;
            }
            else if(RecBuf != NULL)
            {
                //memset(RecBuf, 0, strlen(RecBuf));
                memset(RecBuf, 0, size);
                free(RecBuf);
                RecBuf = NULL;
                LogToFile("In Receive", "Realloc Fail", 1, errno);
                break;
            }
        }
        else
        {
            if(RecBuf == NULL)
                break;
            if(OutCnt > 5)//2에서 5로 바꿨다.
            {
                strcpy(RecBuf, "NULL\n");
                *rsize = 0;
                LogToFile("In Receive", "Time Out", 0, 0);
                break;
            }
            OutCnt++;
        }
        //ioctl(Sock, FIONREAD, &re);
    }
    while(buf[len-1] != '\0');
    return RecBuf;
}

