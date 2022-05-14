server_partial_file_request(struct httpd *env, struct client *clt, char *path,
    struct stat *st, char *range_str)
{
	struct server_config	*srv_conf = clt->clt_srv_conf;
 	struct http_descriptor	*resp = clt->clt_descresp;
 	struct http_descriptor	*desc = clt->clt_descreq;
 	struct media_type	*media, multipart_media;
	struct range_data	*r = &clt->clt_ranges;
 	struct range		*range;
	size_t			 content_length = 0;
 	int			 code = 500, fd = -1, i, nranges, ret;
 	char			 content_range[64];
 	const char		*errstr = NULL;
 
 	/* Ignore range request for methods other than GET */
 	if (desc->http_method != HTTP_METHOD_GET)
 		return server_file_request(env, clt, path, st);
 
	if ((nranges = parse_ranges(clt, range_str, st->st_size)) < 1) {
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
	r->range_media = media;
 
 	if (nranges == 1) {
		range = &r->range[0];
 		(void)snprintf(content_range, sizeof(content_range),
 		    "bytes %lld-%lld/%lld", range->start, range->end,
 		    st->st_size);
 		if (kv_add(&resp->http_headers, "Content-Range",
 		    content_range) == NULL)
 			goto abort;
 
		range = &r->range[0];
		content_length += range->end - range->start + 1;
 	} else {
		/* Add boundary, all parts will be handled by the callback */
		arc4random_buf(&clt->clt_boundary, sizeof(clt->clt_boundary));
 
		/* Calculate Content-Length of the complete multipart body */
		for (i = 0; i < nranges; i++) {
			range = &r->range[i];
 
			/* calculate Content-Length of the complete body */
			if ((ret = snprintf(NULL, 0,
			    "\r\n--%llu\r\n"
			    "Content-Type: %s/%s\r\n"
 			    "Content-Range: bytes %lld-%lld/%lld\r\n\r\n",
			    clt->clt_boundary,
			    media->media_type, media->media_subtype,
			    range->start, range->end, st->st_size)) < 0)
 				goto abort;
 
			/* Add data length */
			content_length += ret + range->end - range->start + 1;
 
 		}
		if ((ret = snprintf(NULL, 0, "\r\n--%llu--\r\n",
		    clt->clt_boundary)) < 0)
 			goto abort;
		content_length += ret;
 
 		/* prepare multipart/byteranges media type */
 		(void)strlcpy(multipart_media.media_type, "multipart",
 		    sizeof(multipart_media.media_type));
 		(void)snprintf(multipart_media.media_subtype,
 		    sizeof(multipart_media.media_subtype),
		    "byteranges; boundary=%llu", clt->clt_boundary);
 		media = &multipart_media;
 	}
 
	/* Start with first range */
	r->range_toread = TOREAD_HTTP_RANGE;
 
 	ret = server_response_http(clt, 206, media, content_length,
 	    MINIMUM(time(NULL), st->st_mtim.tv_sec));
	switch (ret) {
	case -1:
 		goto fail;
 	case 0:
 		/* Connection is already finished */
		close(fd);
 		goto done;
 	default:
 		break;
 	}
 
	clt->clt_fd = fd;
	if (clt->clt_srvbev != NULL)
		bufferevent_free(clt->clt_srvbev);

	clt->clt_srvbev_throttled = 0;
	clt->clt_srvbev = bufferevent_new(clt->clt_fd, server_read_httprange,
	    server_write, server_file_error, clt);
	if (clt->clt_srvbev == NULL) {
		errstr = "failed to allocate file buffer event";
 		goto fail;
	}
 
	/* Adjust read watermark to the socket output buffer size */
	bufferevent_setwatermark(clt->clt_srvbev, EV_READ, 0,
	    clt->clt_sndbufsiz);

	bufferevent_settimeout(clt->clt_srvbev,
	    srv_conf->timeout.tv_sec, srv_conf->timeout.tv_sec);
	bufferevent_enable(clt->clt_srvbev, EV_READ);
	bufferevent_disable(clt->clt_bev, EV_READ);
 
  done:
 	server_reset_http(clt);
 	return (0);
  fail:
 	bufferevent_disable(clt->clt_bev, EV_READ|EV_WRITE);
 	bufferevent_free(clt->clt_bev);
 	clt->clt_bev = NULL;
  abort:
 	if (fd != -1)
 		close(fd);
 	if (errstr == NULL)
		errstr = strerror(errno);
	server_abort_http(clt, code, errstr);
	return (-1);
}
