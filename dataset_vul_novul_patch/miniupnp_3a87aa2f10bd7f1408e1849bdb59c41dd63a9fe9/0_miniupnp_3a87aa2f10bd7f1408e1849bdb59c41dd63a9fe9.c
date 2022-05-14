getHTTPResponse(int s, int * size)
{
	char buf[2048];
	int n;
	int endofheaders = 0;
	int chunked = 0;
	int content_length = -1;
	unsigned int chunksize = 0;
	unsigned int bytestocopy = 0;
	/* buffers : */
	char * header_buf;
	unsigned int header_buf_len = 2048;
	unsigned int header_buf_used = 0;
	char * content_buf;
	unsigned int content_buf_len = 2048;
	unsigned int content_buf_used = 0;
	char chunksize_buf[32];
	unsigned int chunksize_buf_index;

	header_buf = malloc(header_buf_len);
	content_buf = malloc(content_buf_len);
	chunksize_buf[0] = '\0';
	chunksize_buf_index = 0;

	while((n = receivedata(s, buf, 2048, 5000, NULL)) > 0)
	{
		if(endofheaders == 0)
		{
			int i;
			int linestart=0;
			int colon=0;
			int valuestart=0;
			if(header_buf_used + n > header_buf_len) {
				header_buf = realloc(header_buf, header_buf_used + n);
				header_buf_len = header_buf_used + n;
			}
			memcpy(header_buf + header_buf_used, buf, n);
			header_buf_used += n;
			/* search for CR LF CR LF (end of headers)
			 * recognize also LF LF */
			i = 0;
			while(i < ((int)header_buf_used-1) && (endofheaders == 0)) {
				if(header_buf[i] == '\r') {
					i++;
					if(header_buf[i] == '\n') {
						i++;
						if(i < (int)header_buf_used && header_buf[i] == '\r') {
							i++;
							if(i < (int)header_buf_used && header_buf[i] == '\n') {
								endofheaders = i+1;
							}
						}
					}
				} else if(header_buf[i] == '\n') {
					i++;
					if(header_buf[i] == '\n') {
						endofheaders = i+1;
					}
				}
				i++;
			}
			if(endofheaders == 0)
				continue;
			/* parse header lines */
			for(i = 0; i < endofheaders - 1; i++) {
				if(colon <= linestart && header_buf[i]==':')
				{
					colon = i;
					while(i < (endofheaders-1)
					      && (header_buf[i+1] == ' ' || header_buf[i+1] == '\t'))
						i++;
					valuestart = i + 1;
				}
				/* detecting end of line */
				else if(header_buf[i]=='\r' || header_buf[i]=='\n')
				{
					if(colon > linestart && valuestart > colon)
					{
#ifdef DEBUG
						printf("header='%.*s', value='%.*s'\n",
						       colon-linestart, header_buf+linestart,
						       i-valuestart, header_buf+valuestart);
#endif
						if(0==strncasecmp(header_buf+linestart, "content-length", colon-linestart))
						{
							content_length = atoi(header_buf+valuestart);
#ifdef DEBUG
							printf("Content-Length: %d\n", content_length);
#endif
						}
						else if(0==strncasecmp(header_buf+linestart, "transfer-encoding", colon-linestart)
						   && 0==strncasecmp(header_buf+valuestart, "chunked", 7))
						{
#ifdef DEBUG
							printf("chunked transfer-encoding!\n");
#endif
 							chunked = 1;
 						}
 					}
					while((i < (int)header_buf_used) && (header_buf[i]=='\r' || header_buf[i] == '\n'))
 						i++;
 					linestart = i;
 					colon = linestart;
					valuestart = 0;
				}
			}
			/* copy the remaining of the received data back to buf */
			n = header_buf_used - endofheaders;
			memcpy(buf, header_buf + endofheaders, n);
			/* if(headers) */
		}
		if(endofheaders)
		{
			/* content */
			if(chunked)
			{
				int i = 0;
				while(i < n)
				{
					if(chunksize == 0)
					{
						/* reading chunk size */
						if(chunksize_buf_index == 0) {
							/* skipping any leading CR LF */
							if(i<n && buf[i] == '\r') i++;
							if(i<n && buf[i] == '\n') i++;
						}
						while(i<n && isxdigit(buf[i])
						     && chunksize_buf_index < (sizeof(chunksize_buf)-1))
						{
							chunksize_buf[chunksize_buf_index++] = buf[i];
							chunksize_buf[chunksize_buf_index] = '\0';
							i++;
						}
						while(i<n && buf[i] != '\r' && buf[i] != '\n')
							i++; /* discarding chunk-extension */
						if(i<n && buf[i] == '\r') i++;
						if(i<n && buf[i] == '\n') {
							unsigned int j;
							for(j = 0; j < chunksize_buf_index; j++) {
							if(chunksize_buf[j] >= '0'
							   && chunksize_buf[j] <= '9')
								chunksize = (chunksize << 4) + (chunksize_buf[j] - '0');
							else
								chunksize = (chunksize << 4) + ((chunksize_buf[j] | 32) - 'a' + 10);
							}
							chunksize_buf[0] = '\0';
							chunksize_buf_index = 0;
							i++;
						} else {
							/* not finished to get chunksize */
							continue;
						}
#ifdef DEBUG
						printf("chunksize = %u (%x)\n", chunksize, chunksize);
#endif
						if(chunksize == 0)
						{
#ifdef DEBUG
							printf("end of HTTP content - %d %d\n", i, n);
							/*printf("'%.*s'\n", n-i, buf+i);*/
#endif
							goto end_of_stream;
						}
					}
					bytestocopy = ((int)chunksize < (n - i))?chunksize:(unsigned int)(n - i);
					if((content_buf_used + bytestocopy) > content_buf_len)
					{
						if(content_length >= (int)(content_buf_used + bytestocopy)) {
							content_buf_len = content_length;
						} else {
							content_buf_len = content_buf_used + bytestocopy;
						}
						content_buf = (char *)realloc((void *)content_buf,
						                              content_buf_len);
					}
					memcpy(content_buf + content_buf_used, buf + i, bytestocopy);
					content_buf_used += bytestocopy;
					i += bytestocopy;
					chunksize -= bytestocopy;
				}
			}
			else
			{
				/* not chunked */
				if(content_length > 0
				   && (int)(content_buf_used + n) > content_length) {
					/* skipping additional bytes */
					n = content_length - content_buf_used;
				}
				if(content_buf_used + n > content_buf_len)
				{
					if(content_length >= (int)(content_buf_used + n)) {
						content_buf_len = content_length;
					} else {
						content_buf_len = content_buf_used + n;
					}
					content_buf = (char *)realloc((void *)content_buf,
					                              content_buf_len);
				}
				memcpy(content_buf + content_buf_used, buf, n);
				content_buf_used += n;
			}
		}
		/* use the Content-Length header value if available */
		if(content_length > 0 && (int)content_buf_used >= content_length)
		{
#ifdef DEBUG
			printf("End of HTTP content\n");
#endif
			break;
		}
	}
end_of_stream:
	free(header_buf); header_buf = NULL;
	*size = content_buf_used;
	if(content_buf_used == 0)
	{
		free(content_buf);
		content_buf = NULL;
	}
	return content_buf;
}
