
int Init_FIFO(char *Path);

int Init_FIFO(char *Path)
{
	if(access(Path, F_OK) == 0)
		unlink(Path);
    mkfifo(Path, 0666);
    return open(Path, O_WRONLY);
}

