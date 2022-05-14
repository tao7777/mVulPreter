server_partial_file_request(struct httpd *env, struct client *clt, char *path,
    struct stat *st, char *range_str)
{
	struct server_config	*srv_conf = clt->clt_srv_conf;
 	struct http_descriptor	*resp = clt->clt_descresp;
 	struct http_descriptor	*desc = clt->clt_descreq;
 	struct media_type	*media, multipart_media;
 	struct range		*range;
	struct evbuffer		*evb = NULL;
	size_t			 content_length;
 	int			 code = 500, fd = -1, i, nranges, ret;
	uint32_t		 boundary;
 	char			 content_range[64];
 	const char		*errstr = NULL;
 
 	/* Ignore range request for methods other than GET */
 	if (desc->http_method != HTTP_METHOD_GET)
 		return server_file_request(env, clt, path, st);
 
	if ((range = parse_range(range_str, st->st_size, &nranges)) == NULL) {
 		code = 416;
 		(void)snprintf(content_range, sizeof(content_range),
 		    "bytes */%lld", st->st_size);
		errstr = content_range;
		goto abort;
	}

	/* Now open the file, should be readable or we have another problem */
	if ((fd = open(path, O_RDONLY)) == -1)
 		goto abort;
 
 	media = media_find_config(env, srv_conf, path);
	if ((evb = evbuffer_new()) == NULL) {
		errstr = "failed to allocate file buffer";
		goto abort;
	}
 
 	if (nranges == 1) {
 		(void)snprintf(content_range, sizeof(content_range),
 		    "bytes %lld-%lld/%lld", range->start, range->end,
 		    st->st_size);
 		if (kv_add(&resp->http_headers, "Content-Range",
 		    content_range) == NULL)
 			goto abort;
 
		content_length = range->end - range->start + 1;
		if (buffer_add_range(fd, evb, range) == 0)
			goto abort;
 	} else {
		content_length = 0;
		boundary = arc4random();
		/* Generate a multipart payload of byteranges */
		while (nranges--) {
			if ((i = evbuffer_add_printf(evb, "\r\n--%ud\r\n",
			    boundary)) == -1)
				goto abort;
 
			content_length += i;
			if ((i = evbuffer_add_printf(evb,
			    "Content-Type: %s/%s\r\n",
			    media->media_type, media->media_subtype)) == -1)
				goto abort;
 
			content_length += i;
			if ((i = evbuffer_add_printf(evb,
 			    "Content-Range: bytes %lld-%lld/%lld\r\n\r\n",
			    range->start, range->end, st->st_size)) == -1)
 				goto abort;
 
			content_length += i;
			if (buffer_add_range(fd, evb, range) == 0)
				goto abort;
 
			content_length += range->end - range->start + 1;
			range++;
 		}
		if ((i = evbuffer_add_printf(evb, "\r\n--%ud--\r\n",
		    boundary)) == -1)
 			goto abort;
		content_length += i;
 
 		/* prepare multipart/byteranges media type */
 		(void)strlcpy(multipart_media.media_type, "multipart",
 		    sizeof(multipart_media.media_type));
 		(void)snprintf(multipart_media.media_subtype,
 		    sizeof(multipart_media.media_subtype),
		    "byteranges; boundary=%ud", boundary);
 		media = &multipart_media;
 	}
 
	close(fd);
	fd = -1;
 
 	ret = server_response_http(clt, 206, media, content_length,
 	    MINIMUM(time(NULL), st->st_mtim.tv_sec));
	switch (ret) {
	case -1:
 		goto fail;
 	case 0:
 		/* Connection is already finished */
 		goto done;
 	default:
 		break;
 	}
 
	if (server_bufferevent_write_buffer(clt, evb) == -1)
 		goto fail;
 
	bufferevent_enable(clt->clt_bev, EV_READ|EV_WRITE);
	if (clt->clt_persist)
		clt->clt_toread = TOREAD_HTTP_HEADER;
	else
		clt->clt_toread = TOREAD_HTTP_NONE;
	clt->clt_done = 0;
 
  done:
	evbuffer_free(evb);
 	server_reset_http(clt);
 	return (0);
  fail:
 	bufferevent_disable(clt->clt_bev, EV_READ|EV_WRITE);
 	bufferevent_free(clt->clt_bev);
 	clt->clt_bev = NULL;
  abort:
	if (evb != NULL)
		evbuffer_free(evb);
 	if (fd != -1)
 		close(fd);
 	if (errstr == NULL)
		errstr = strerror(errno);
	server_abort_http(clt, code, errstr);
	return (-1);
}
