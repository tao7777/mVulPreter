static char *print_number( cJSON *item )
static int update(printbuffer *p)
 {
 	char *str;
	if (!p || !p->buffer) return 0;
	str=p->buffer+p->offset;
	return p->offset+strlen(str);
}
 
/* Render the number nicely from the given item into a string. */
static char *print_number(cJSON *item,printbuffer *p)
{
	char *str=0;
	double d=item->valuedouble;
	if (d==0)
	{
		if (p)	str=ensure(p,2);
		else	str=(char*)cJSON_malloc(2);	/* special case for 0. */
		if (str) strcpy(str,"0");
	}
	else if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=LLONG_MAX && d>=LLONG_MIN)
	{
		if (p)	str=ensure(p,64);
		else	str=(char*)cJSON_malloc(64);
		if (str)	sprintf(str,"%lld",(long long) item->valueint);
	}
	else
	{
		if (p)	str=ensure(p,64);
		else	str=(char*)cJSON_malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fpclassify(d) != FP_ZERO && !isnormal(d))				sprintf(str,"null");
			else if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)	sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)					sprintf(str,"%e",d);
			else														sprintf(str,"%f",d);
		}
 	}
 	return str;
 }
