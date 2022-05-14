static const char *parse_value( cJSON *item, const char *value )
static const char *parse_value(cJSON *item,const char *value,const char **ep)
 {
	if (!value)						return 0;	/* Fail on null. */
	if (!strncmp(value,"null",4))	{ item->type=cJSON_NULL;  return value+4; }
	if (!strncmp(value,"false",5))	{ item->type=cJSON_False; return value+5; }
	if (!strncmp(value,"true",4))	{ item->type=cJSON_True; item->valueint=1;	return value+4; }
	if (*value=='\"')				{ return parse_string(item,value,ep); }
	if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
	if (*value=='[')				{ return parse_array(item,value,ep); }
	if (*value=='{')				{ return parse_object(item,value,ep); }
 
	*ep=value;return 0;	/* failure. */
 }
 
 /* Render a value to text. */
static char *print_value(cJSON *item,int depth,int fmt,printbuffer *p)
{
	char *out=0;
	if (!item) return 0;
	if (p)
	{
		switch ((item->type)&255)
		{
			case cJSON_NULL:	{out=ensure(p,5);	if (out) strcpy(out,"null");	break;}
			case cJSON_False:	{out=ensure(p,6);	if (out) strcpy(out,"false");	break;}
			case cJSON_True:	{out=ensure(p,5);	if (out) strcpy(out,"true");	break;}
			case cJSON_Number:	out=print_number(item,p);break;
			case cJSON_String:	out=print_string(item,p);break;
			case cJSON_Array:	out=print_array(item,depth,fmt,p);break;
			case cJSON_Object:	out=print_object(item,depth,fmt,p);break;
		}
	}
	else
	{
		switch ((item->type)&255)
		{
			case cJSON_NULL:	out=cJSON_strdup("null");	break;
			case cJSON_False:	out=cJSON_strdup("false");break;
			case cJSON_True:	out=cJSON_strdup("true"); break;
			case cJSON_Number:	out=print_number(item,0);break;
			case cJSON_String:	out=print_string(item,0);break;
			case cJSON_Array:	out=print_array(item,depth,fmt,0);break;
			case cJSON_Object:	out=print_object(item,depth,fmt,0);break;
		}
 	}
 	return out;
 }
 
 /* Build an array from input text. */
static const char *parse_array(cJSON *item,const char *value,const char **ep)
 {
 	cJSON *child;
	if (*value!='[')	{*ep=value;return 0;}	/* not an array! */
 
	item->type=cJSON_Array;
	value=skip(value+1);
	if (*value==']') return value+1;	/* empty array. */
 
	item->child=child=cJSON_New_Item();
	if (!item->child) return 0;		 /* memory fail */
	value=skip(parse_value(child,skip(value),ep));	/* skip any spacing, get the value. */
	if (!value) return 0;
 
	while (*value==',')
	{
 		cJSON *new_item;
		if (!(new_item=cJSON_New_Item())) return 0; 	/* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1),ep));
		if (!value) return 0;	/* memory fail */
 	}
 
	if (*value==']') return value+1;	/* end of array */
	*ep=value;return 0;	/* malformed. */
 }
 
 /* Render an array to text */
static char *print_array(cJSON *item,int depth,int fmt,printbuffer *p)
 {
 	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	cJSON *child=item->child;
	int numentries=0,i=0,fail=0;
	size_t tmplen=0;
 	
 	/* How many entries in the array? */
	while (child) numentries++,child=child->next;
	/* Explicitly handle numentries==0 */
	if (!numentries)
	{
		if (p)	out=ensure(p,3);
		else	out=(char*)cJSON_malloc(3);
		if (out) strcpy(out,"[]");
		return out;
	}

	if (p)
	{
		/* Compose the output array. */
		i=p->offset;
		ptr=ensure(p,1);if (!ptr) return 0;	*ptr='[';	p->offset++;
		child=item->child;
		while (child && !fail)
		{
			print_value(child,depth+1,fmt,p);
			p->offset=update(p);
			if (child->next) {len=fmt?2:1;ptr=ensure(p,len+1);if (!ptr) return 0;*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;p->offset+=len;}
			child=child->next;
		}
		ptr=ensure(p,2);if (!ptr) return 0;	*ptr++=']';*ptr=0;
		out=(p->buffer)+i;
 	}
	else
	{
		/* Allocate an array to hold the values for each */
		entries=(char**)cJSON_malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		memset(entries,0,numentries*sizeof(char*));
		/* Retrieve all the results: */
		child=item->child;
		while (child && !fail)
		{
			ret=print_value(child,depth+1,fmt,0);
			entries[i++]=ret;
			if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
			child=child->next;
 		}
		
		/* If we didn't fail, try to malloc the output string */
		if (!fail)	out=(char*)cJSON_malloc(len);
		/* If that fails, we fail. */
		if (!out) fail=1;

		/* Handle failure. */
		if (fail)
		{
			for (i=0;i<numentries;i++) if (entries[i]) cJSON_free(entries[i]);
			cJSON_free(entries);
			return 0;
		}
		
		/* Compose the output array. */
		*out='[';
		ptr=out+1;*ptr=0;
		for (i=0;i<numentries;i++)
		{
			tmplen=strlen(entries[i]);memcpy(ptr,entries[i],tmplen);ptr+=tmplen;
			if (i!=numentries-1) {*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;}
			cJSON_free(entries[i]);
		}
		cJSON_free(entries);
		*ptr++=']';*ptr++=0;
 	}
 	return out;	
 }
 
 /* Build an object from the text. */
static const char *parse_object(cJSON *item,const char *value,const char **ep)
 {
 	cJSON *child;
	if (*value!='{')	{*ep=value;return 0;}	/* not an object! */
 	
	item->type=cJSON_Object;
	value=skip(value+1);
	if (*value=='}') return value+1;	/* empty array. */
 	
	item->child=child=cJSON_New_Item();
	if (!item->child) return 0;
	value=skip(parse_string(child,skip(value),ep));
	if (!value) return 0;
	child->string=child->valuestring;child->valuestring=0;
	if (*value!=':') {*ep=value;return 0;}	/* fail! */
	value=skip(parse_value(child,skip(value+1),ep));	/* skip any spacing, get the value. */
	if (!value) return 0;
 	
	while (*value==',')
	{
 		cJSON *new_item;
		if (!(new_item=cJSON_New_Item()))	return 0; /* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_string(child,skip(value+1),ep));
		if (!value) return 0;
		child->string=child->valuestring;child->valuestring=0;
		if (*value!=':') {*ep=value;return 0;}	/* fail! */
		value=skip(parse_value(child,skip(value+1),ep));	/* skip any spacing, get the value. */
		if (!value) return 0;
 	}
 	
	if (*value=='}') return value+1;	/* end of array */
	*ep=value;return 0;	/* malformed. */
 }
