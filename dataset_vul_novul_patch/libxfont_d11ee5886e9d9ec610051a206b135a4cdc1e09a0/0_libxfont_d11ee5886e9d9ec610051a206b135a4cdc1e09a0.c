BufCompressedFill (BufFilePtr f)
{
    CompressedFile  *file;
    register char_type *stackp, *de_stack;
    register char_type finchar;
    register code_int code, oldcode, incode;
    BufChar	    *buf, *bufend;

    file = (CompressedFile *) f->private;

    buf = f->buffer;
    bufend = buf + BUFFILESIZE;
    stackp = file->stackp;
    de_stack = file->de_stack;
    finchar = file->finchar;
    oldcode = file->oldcode;
    while (buf < bufend) {
	while (stackp > de_stack && buf < bufend)
	    *buf++ = *--stackp;

	if (buf == bufend)
	    break;

	if (oldcode == -1)
	    break;

	code = getcode (file);
	if (code == -1)
	    break;
    
    	if ( (code == CLEAR) && file->block_compress ) {
	    for ( code = 255; code >= 0; code-- )
	    	file->tab_prefix[code] = 0;
	    file->clear_flg = 1;
	    file->free_ent = FIRST - 1;
	    if ( (code = getcode (file)) == -1 )	/* O, untimely death! */
	    	break;
    	}
    	incode = code;
    	/*
     	 * Special case for KwKwK string.
     	 */
    	if ( code >= file->free_ent ) {
	    *stackp++ = finchar;
	    code = oldcode;
    	}
    
    	/*
     	 * Generate output characters in reverse order
      	 */
     	while ( code >= 256 )
     	{
	    if (stackp - de_stack >= STACK_SIZE - 1)
		return BUFFILEEOF;
 	    *stackp++ = file->tab_suffix[code];
 	    code = file->tab_prefix[code];
     	}
    
    	/*
     	 * Generate the new entry.
     	 */
    	if ( (code=file->free_ent) < file->maxmaxcode ) {
	    file->tab_prefix[code] = (unsigned short)oldcode;
	    file->tab_suffix[code] = finchar;
	    file->free_ent = code+1;
    	} 
	/*
	 * Remember previous code.
	 */
	oldcode = incode;
    }
    file->oldcode = oldcode;
    file->stackp = stackp;
    file->finchar = finchar;
    if (buf == f->buffer) {
	f->left = 0;
	return BUFFILEEOF;
    }
    f->bufp = f->buffer + 1;
    f->left = (buf - f->buffer) - 1;
    return f->buffer[0];
}
