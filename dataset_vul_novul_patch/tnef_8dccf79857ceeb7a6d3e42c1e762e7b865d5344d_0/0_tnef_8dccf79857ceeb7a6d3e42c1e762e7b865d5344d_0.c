get_html_data (MAPI_Attr *a)
{
    VarLenData **body = XCALLOC(VarLenData*, a->num_values + 1);

     int j;
     for (j = 0; j < a->num_values; j++)
     {
        if (a->type == szMAPI_BINARY) {
 	    body[j] = XMALLOC(VarLenData, 1);
	    body[j]->len = a->values[j].len;
	    body[j]->data = CHECKED_XCALLOC(unsigned char, a->values[j].len);
	    memmove (body[j]->data, a->values[j].data.buf, body[j]->len);
        }
     }
     return body;
 }
