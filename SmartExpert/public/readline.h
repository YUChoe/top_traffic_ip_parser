#ifndef _READLINE_HEADER_
#define _READLINE_HEADER_
int Readline(int fd, char *Buf, int Max, int newfd);
int _readline(int fd, char *ptr, int newfd);

int Readline(int fd, char *Buf, int Max, int newfd) //널포함 카운트 리턴
{
	int i = 1;
	int ret;

	do
	{
		ret = _readline(fd, Buf, newfd);
		if(ret == 0)
		{
			if(i == 1)
				return(0);
			else
				*Buf = '\n';
		}
		else if(ret == -1)
			return(-1);
	}while((i++ < Max) && (*Buf++ != '\n'));
	*(Buf-1) = 0;
	return(i-1);
}

int _readline(int fd, char *ptr, int newfd)
{
	static char rbuf[255];
	static char *ch;
	static int cnt = 0;

	if(newfd)
	{
		cnt = 0;
		return 0;
	}
	if(cnt <= 0)
	{
		again:
		if((cnt = read(fd, rbuf, 255)) < 0)
		{
			if(errno == EINTR)
				goto again;
				return(-1);
		}
		else if(cnt == 0)
			return(0);
		ch = rbuf;
	}
	cnt--;
	*ptr = *ch++;
	return(1);
}
#endif
