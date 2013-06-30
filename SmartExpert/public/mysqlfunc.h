short Init_MySQL(MYSQL *mysql, char *dbaddr, char *user, char *pass, char *dbname)
{
	mysql_init(mysql);
	if(!mysql_real_connect(mysql, dbaddr, user, pass, dbname, 0, NULL, 0))
	{
		LogToFile("Init_MySQL", (char *)mysql_error(mysql), 0, 0);
		return 0;
	}
	return 1;
}


char *GetQuery(MYSQL *mysql, char *Query, char *PreCode, int PreCodeNum)
{
	char *Ret = NULL, *tmp = NULL;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int i, fields;
	int slen = PreCodeNum, tlen = 0;

	if(mysql_query(mysql, Query))
		LogToFile("GetQuery", (char *)mysql_error(mysql), 0, 0);

	if((PreCodeNum > 0) && (PreCode != NULL))
	{
		Ret = (char *)malloc(slen + 1);
		memset(Ret, 0, slen+1);
		strcpy(Ret, PreCode);
	}

	res = mysql_store_result(mysql);
	if(mysql_num_rows(res))
	{
		fields = mysql_num_fields(res);
		while(row = mysql_fetch_row(res))
		{
			for(i = 0; i < fields; i++)
			{
				if(row[i] != NULL)
					tlen += strlen(row[i]) + 1;
				else
					tlen += 3;
			}
			if((tmp = (char *)realloc(Ret, slen + tlen +1)) == NULL)
			{
				Free(Ret);
				return NULL;
			}
			Ret = tmp;
			for(i = 0; i < fields; i++)
			{
				if(row[i] != NULL)
				{
					strcpy(Ret+slen, row[i]);
					slen += strlen(row[i]);
				}
				*(Ret+slen) = '\n';
				slen++;
			}
			*(Ret+slen-1) = '\n';
			tlen = 0;
		}
		*(Ret+slen) = '\0';
		mysql_free_result(res);
	}
	//else
	//  {
	//      Ret = (char *)malloc(slen+1);
	//      memset(Ret, 0, slen+1);
	//  }
	return Ret;
}

