#include <setjmp.h>
#include <errno.h>
#include <sys/stat.h>

extern int loop;
sigjmp_buf jmp;

short Parsing(char **Point);
void Writepid(int pid, char *Path);
int Getword(char *Buf, char **Point, int len);
char *GetWordToPointer(char **Point);
void Sighandle(int sig);
char *ReadFile(char *path);

char *ReadFile(char *path)
{
	int fd;
	char *Ret = NULL;
	struct stat st;

	if((fd = open(path, O_RDONLY)) <= 0)
		return NULL;
	
	if(fstat(fd, &st))
	{
		close(fd);
		return NULL;
	}
	Ret = (char *)malloc(st.st_size+1);
	memset(Ret, 0, st.st_size+1);
	if(read(fd, Ret, st.st_size) != st.st_size)
	{
		Free(Ret);
		close(fd);
		return NULL;
	}
	close(fd);
	return Ret;
}

void Sighandle(int sig)
{
	//printf("Get Signal\n");
	loop = 0;
	siglongjmp(jmp, 1);
}

char *GetWordToPointer(char **Point)
{
	char *Ret = NULL;
	int i = 0;

	if(**Point == '\0')
		return NULL;
		
	while((*((*Point)+i) != '\n') && (*((*Point)+i) != '\0'))
		i++;
	if(i == 0)
		return NULL;
	
	Ret = (char *)malloc(i+1);
	memset(Ret, 0, i+1);
	strncpy(Ret, *Point, i);
	*Point += i+1;

	return Ret;
}

int Getword(char *Buf, char **Point, int len)
{
	int rlen = 0;
	char *tmp = Buf;
	
	if(**Point == '\0')
	{
		*Buf = '\0';
		return -1;
	}
	while((**Point != '\n') && (rlen < (len - 1)))
	{
		*(Buf++) = *((*Point)++);
		rlen++;
		if(**Point == '\0')
			return 0;
	}
	
	*Buf = '\0';
	++(*Point);
	if(tmp == Buf)//(*(++(*Point)) == '\0') || (tmp == Buf))
		return 0;
	return 1;
}

short Parsing(char **Point)
{
    char code[2];
    if( ((*Point)[0] < '0') || ((*Point)[0] > '9') || ((*Point)[1] != '\n') )
        return -1;
    memcpy(code, *Point, 2);
    code[1] = '\0';
    (*Point)++;
    (*Point)++;
    return atoi(code);
}

void Writepid(int pid, char *Path)
{
    FILE *fp;

    fp = fopen(Path, "w");
    fprintf(fp, "%d\n", pid);
    fclose(fp);
}

