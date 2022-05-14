file_add_mapi_attrs (File* file, MAPI_Attr** attrs)
{
    int i;
    for (i = 0; attrs[i]; i++)
    {
	MAPI_Attr* a = attrs[i];

	if (a->num_values)
	{

 	    switch (a->name)
 	    {
 	    case MAPI_ATTACH_LONG_FILENAME:
		assert(a->type == szMAPI_STRING);
 		if (file->name) XFREE(file->name);
 		file->name = strdup( (char*)a->values[0].data.buf );
 		break;
 
 	    case MAPI_ATTACH_DATA_OBJ:
		assert((a->type == szMAPI_BINARY) || (a->type == szMAPI_OBJECT));
 		file->len = a->values[0].len;
 		if (file->data) XFREE (file->data);
 		file->data = CHECKED_XMALLOC (unsigned char, file->len);
 		memmove (file->data, a->values[0].data.buf, file->len);
 		break;
 
              case MAPI_ATTACH_MIME_TAG:
		assert(a->type == szMAPI_STRING);
 		if (file->mime_type) XFREE (file->mime_type);
 		file->mime_type = CHECKED_XMALLOC (char, a->values[0].len);
 		memmove (file->mime_type, a->values[0].data.buf, a->values[0].len);
 		break;
 
                 case MAPI_ATTACH_CONTENT_ID:
                    assert(a->type == szMAPI_STRING);
                     if (file->content_id) XFREE(file->content_id);
                     file->content_id = CHECKED_XMALLOC (char, a->values[0].len);
                     memmove (file->content_id, a->values[0].data.buf, a->values[0].len);
                    break;

	    default:
		break;
	    }
	}
    }
}
