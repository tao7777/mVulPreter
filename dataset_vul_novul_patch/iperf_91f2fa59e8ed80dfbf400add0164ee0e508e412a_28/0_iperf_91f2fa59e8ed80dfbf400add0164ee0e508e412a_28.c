char *cJSON_PrintUnformatted( cJSON *item )
char *cJSON_Print(cJSON *item)				{return print_value(item,0,1,0);}
char *cJSON_PrintUnformatted(cJSON *item)	{return print_value(item,0,0,0);}

char *cJSON_PrintBuffered(cJSON *item,int prebuffer,int fmt)
 {
	printbuffer p;
	p.buffer=(char*)cJSON_malloc(prebuffer);
	p.length=prebuffer;
	p.offset=0;
	return print_value(item,0,fmt,&p);
 }
