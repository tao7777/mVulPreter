mapi_attr_read (size_t len, unsigned char *buf)
{
    size_t idx = 0;
     uint32 i,j;
     assert(len > 4);
     uint32 num_properties = GETINT32(buf+idx);
    assert((num_properties+1) != 0);
     MAPI_Attr** attrs = CHECKED_XMALLOC (MAPI_Attr*, (num_properties + 1));
 
     idx += 4;

    if (!attrs) return NULL;
    for (i = 0; i < num_properties; i++)
    {
	MAPI_Attr* a = attrs[i] = CHECKED_XCALLOC(MAPI_Attr, 1);
	MAPI_Value* v = NULL;

	CHECKINT16(idx, len); a->type = GETINT16(buf+idx); idx += 2;
	CHECKINT16(idx, len); a->name = GETINT16(buf+idx); idx += 2;

	/* handle special case of GUID prefixed properties */
	if (a->name & GUID_EXISTS_FLAG)
	{
	    /* copy GUID */
	    a->guid = CHECKED_XMALLOC(GUID, 1);
	    copy_guid_from_buf(a->guid, buf+idx, len);
	    idx += sizeof (GUID);

	    CHECKINT32(idx, len); a->num_names = GETINT32(buf+idx); idx += 4;
	    if (a->num_names > 0)
	    {
		/* FIXME: do something useful here! */
		size_t i;

		a->names = CHECKED_XCALLOC(VarLenData, a->num_names);

		for (i = 0; i < a->num_names; i++)
		{
		    size_t j;

		    CHECKINT32(idx, len); a->names[i].len = GETINT32(buf+idx); idx += 4;

 		    /* read the data into a buffer */
 		    a->names[i].data 
 			= CHECKED_XMALLOC(unsigned char, a->names[i].len);
		    assert((idx+(a->names[i].len*2)) <= len);
 		    for (j = 0; j < (a->names[i].len >> 1); j++)
 			a->names[i].data[j] = (buf+idx)[j*2];
 
		    /* But what are we going to do with it? */
		    
		    idx += pad_to_4byte(a->names[i].len);
		}
	    }
	    else
	    {
		/* get the 'real' name */
		CHECKINT32(idx, len); a->name = GETINT32(buf+idx); idx+= 4;
	    }
	}

	/* 
	 * Multi-value types and string/object/binary types have
	 * multiple values 
	 */
	if (a->type & MULTI_VALUE_FLAG ||
	    a->type == szMAPI_STRING ||
	    a->type == szMAPI_UNICODE_STRING ||
	    a->type == szMAPI_OBJECT ||
	    a->type == szMAPI_BINARY)
	{
	    CHECKINT32(idx, len); a->num_values = GETINT32(buf+idx);
	    idx += 4;
	}
        else
        {
	    a->num_values = 1;
        }

	/* Amend the type in case of multi-value type */
	if (a->type & MULTI_VALUE_FLAG)
	{
	    a->type -= MULTI_VALUE_FLAG;
	}


	v = alloc_mapi_values (a);

	for (j = 0; j < a->num_values; j++) 
	{
	    switch (a->type)
	    {
	    case szMAPI_SHORT:	/* 2 bytes */
		v->len = 2;
		CHECKINT16(idx, len); v->data.bytes2 = GETINT16(buf+idx);
		idx += 4;	/* assume padding of 2, advance by 4! */
		break;

	    case szMAPI_INT:	/* 4 bytes */
		v->len = 4;
		CHECKINT32(idx, len); v->data.bytes4 = GETINT32(buf+idx);
		idx += 4;
		v++;
		break;

	    case szMAPI_FLOAT:	/* 4 bytes */
	    case szMAPI_BOOLEAN: /* this should be 2 bytes + 2 padding */
		v->len = 4;
		CHECKINT32(idx, len); v->data.bytes4 = GETINT32(buf+idx);
		idx += v->len;
		break;

	    case szMAPI_SYSTIME: /* 8 bytes */
		v->len = 8;
		CHECKINT32(idx, len); v->data.bytes8[0] = GETINT32(buf+idx);
		CHECKINT32(idx+4, len); v->data.bytes8[1] = GETINT32(buf+idx+4);
		idx += 8;
		v++;
		break;

	    case szMAPI_DOUBLE:	/* 8 bytes */
	    case szMAPI_APPTIME:
	    case szMAPI_CURRENCY:
	    case szMAPI_INT8BYTE:
		v->len = 8;
		CHECKINT32(idx, len); v->data.bytes8[0] = GETINT32(buf+idx);
		CHECKINT32(idx+4, len); v->data.bytes8[1] = GETINT32(buf+idx+4);
		idx += v->len;
		break;

	    case szMAPI_CLSID:
		v->len = sizeof (GUID);
		copy_guid_from_buf(&v->data.guid, buf+idx, len);
		idx += v->len;
		break;

	    case szMAPI_STRING:
	    case szMAPI_UNICODE_STRING:
	    case szMAPI_OBJECT:
 	    case szMAPI_BINARY:
 		CHECKINT32(idx, len); v->len = GETINT32(buf+idx); idx += 4;
 
		assert(v->len + idx <= len);

 		if (a->type == szMAPI_UNICODE_STRING)
 		{
		    assert(v->len != 0);
 		    v->data.buf = (unsigned char*)unicode_to_utf8(v->len, buf+idx);
 		}
 		else
		{
		    v->data.buf = CHECKED_XMALLOC(unsigned char, v->len);
		    memmove (v->data.buf, buf+idx, v->len);
		}

		idx += pad_to_4byte(v->len);
		v++;
		break;

	    case szMAPI_NULL:	/* illegal in input tnef streams */
	    case szMAPI_ERROR:
	    case szMAPI_UNSPECIFIED:
		fprintf (stderr,
			 "Invalid attribute, input file may be corrupted\n");
		if (!ENCODE_SKIP) exit (1);

		return NULL;

	    default:		/* should never get here */
		fprintf (stderr,
			 "Undefined attribute, input file may be corrupted\n");
		if (!ENCODE_SKIP) exit (1);

		return NULL;

	    }
	    if (DEBUG_ON) mapi_attr_dump (attrs[i]);
	}
    }
    attrs[i] = NULL;

    return attrs;
}
