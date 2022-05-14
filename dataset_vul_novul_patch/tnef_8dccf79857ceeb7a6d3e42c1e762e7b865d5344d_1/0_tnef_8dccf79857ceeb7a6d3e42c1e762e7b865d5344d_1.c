parse_file (FILE* input_file, char* directory, 
	    char *body_filename, char *body_pref,
	    int flags)
{
    uint32 d;
    uint16 key;
    Attr *attr = NULL;
    File *file = NULL;
    int rtf_size = 0, html_size = 0;
    MessageBody body;
    memset (&body, '\0', sizeof (MessageBody));

    /* store the program options in our file global variables */
    g_flags = flags;

    /* check that this is in fact a TNEF file */
    d = geti32(input_file);
    if (d != TNEF_SIGNATURE)
    {
	fprintf (stdout, "Seems not to be a TNEF file\n");
	return 1;
    }

    /* Get the key */
    key = geti16(input_file);
    debug_print ("TNEF Key: %hx\n", key);

    /* The rest of the file is a series of 'messages' and 'attachments' */
    while ( data_left( input_file ) )
    {
	attr = read_object( input_file );

	if ( attr == NULL ) break;

	/* This signals the beginning of a file */
	if (attr->name == attATTACHRENDDATA)
	{
	    if (file)
	    {
		file_write (file, directory);
		file_free (file);
	    }
	    else
	    {
		file = CHECKED_XCALLOC (File, 1);
	    }
	}

	/* Add the data to our lists. */
	switch (attr->lvl_type)
	{
	case LVL_MESSAGE:
	    if (attr->name == attBODY)
	    {
		body.text_body = get_text_data (attr);
	    }
	    else if (attr->name == attMAPIPROPS) 
	    { 
		MAPI_Attr **mapi_attrs 
		    = mapi_attr_read (attr->len, attr->buf); 
		if (mapi_attrs)
		{ 
		    int i;
 		    for (i = 0; mapi_attrs[i]; i++)
 		    {
 			MAPI_Attr *a = mapi_attrs[i];
		
			if (a->type == szMAPI_BINARY && a->name == MAPI_BODY_HTML)
 			{
 			    body.html_bodies = get_html_data (a);
                                 html_size = a->num_values;
 			}
			else if (a->type == szMAPI_BINARY && a->name == MAPI_RTF_COMPRESSED)
 			{
 			    body.rtf_bodies = get_rtf_data (a);
                                 rtf_size = a->num_values;
			}
		    }
		    /* cannot save attributes to file, since they
		     * are not attachment attributes */ 
		    /* file_add_mapi_attrs (file, mapi_attrs); */
		    mapi_attr_free_list (mapi_attrs); 
		    XFREE (mapi_attrs); 
		}
	    }
	    break;
	case LVL_ATTACHMENT:
	    file_add_attr (file, attr);
	    break;
	default:
	    fprintf (stderr, "Invalid lvl type on attribute: %d\n",
		     attr->lvl_type);
	    return 1;
	    break;
	}
	attr_free (attr);
	XFREE (attr);
    }

    if (file)
    {
	file_write (file, directory);
	file_free (file);
	XFREE (file);
    }
    
    /* Write the message body */
    if (flags & SAVEBODY)
    {
	int i = 0;
	int all_flag = 0;
	if (strcmp (body_pref, "all") == 0) 
	{
	    all_flag = 1;
	    body_pref = "rht";
	}

	for (; i < 3; i++)
	{
	    File **files
		= get_body_files (body_filename, body_pref[i], &body);
	    if (files)
	    {
		int j = 0; 
		for (; files[j]; j++)
		{
		    file_write(files[j], directory);
		    file_free (files[j]);
                    XFREE(files[j]);
		}
		XFREE(files);
		if (!all_flag) break;
	    }
	}
    }

    if (body.text_body)
    {
        free_bodies(body.text_body, 1);
        XFREE(body.text_body);
    }
    if (rtf_size > 0)
    {
        free_bodies(body.rtf_bodies, rtf_size);
        XFREE(body.rtf_bodies);
    }
    if (html_size > 0)
    {
        free_bodies(body.html_bodies, html_size);
        XFREE(body.html_bodies);
    }
    return 0;
}
