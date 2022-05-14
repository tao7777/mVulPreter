char *my_asctime(time_t t)
{
	struct tm *tm;
	char *str;
         int len;
 
 	tm = localtime(&t);
	if (tm == NULL)
	    return g_strdup("???");

 	str = g_strdup(asctime(tm));
 
 	len = strlen(str);
	if (len > 0) str[len-1] = '\0';
        return str;
}
