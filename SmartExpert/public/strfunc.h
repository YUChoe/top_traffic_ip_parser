char *StrchrEx(char **Org, int chr);
void StrUpper(char *buf);

char *StrchrEx(char **Org, int chr)
{
	char *Ret;

	if((*Org == NULL) || (*(*Org) == '\0'))
		return NULL;
	else
		Ret = *Org;

	if((*Org = strchr(*Org, chr)) != NULL)
	{
		*(*Org) = '\0';
		(*Org)++;
	}

	return Ret;
}

void StrUpper(char *buf)
{
	int i;
	int size = strlen(buf);
	
	for(i = 0; i < size; i++)
	{
		if((97 <= *(buf+i)) && (*(buf+i) <= 122))
			*(buf+i) -= 32;
	}
}
