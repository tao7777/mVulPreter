static const char *parse_string(cJSON *item,const char *str,const char **ep)
 {
 	const char *ptr=str+1,*end_ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc,uc2;
 	if (*str!='\"') {*ep=str;return 0;}	/* not a string! */

	while (*end_ptr!='\"' && *end_ptr && ++len)
	{
	    if (*end_ptr++ == '\\')
	    {
		if (*end_ptr == '\0')
		{
		    /* prevent buffer overflow when last input character is a backslash */
		    return 0;
		}
		end_ptr++;	/* Skip escaped quotes. */
	    }
	}

 	out=(char*)cJSON_malloc(len+1);	/* This is how long we need for the string, roughly. */
 	if (!out) return 0;
 	item->valuestring=out; /* assign here so out will be deleted during cJSON_Delete() later */
	item->type=cJSON_String;
	
	ptr=str+1;ptr2=out;
	while (ptr < end_ptr)
	{
		if (*ptr!='\\') *ptr2++=*ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
				case 'u':	 /* transcode utf16 to utf8. */
					uc=parse_hex4(ptr+1);ptr+=4;	/* get the unicode char. */
					if (ptr >= end_ptr) {*ep=str;return 0;}	/* invalid */
					
					if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)    {*ep=str;return 0;}	/* check for invalid.   */
					
					if (uc>=0xD800 && uc<=0xDBFF)	/* UTF16 surrogate pairs.	*/
					{
						if (ptr+6 > end_ptr)    {*ep=str;return 0;}	/* invalid */
						if (ptr[1]!='\\' || ptr[2]!='u')    {*ep=str;return 0;}	/* missing second-half of surrogate.    */
						uc2=parse_hex4(ptr+3);ptr+=6;
						if (uc2<0xDC00 || uc2>0xDFFF)       {*ep=str;return 0;}	/* invalid second-half of surrogate.    */
						uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
					}

					len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;
					
					switch (len) {
						case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 =(uc | firstByteMark[len]);
					}
					ptr2+=len;
					break;
				default:  *ptr2++=*ptr; break;
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"') ptr++;
	return ptr;
}
