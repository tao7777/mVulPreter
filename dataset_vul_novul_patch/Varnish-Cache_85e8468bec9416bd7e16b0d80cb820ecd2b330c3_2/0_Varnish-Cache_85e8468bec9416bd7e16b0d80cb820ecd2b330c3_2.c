http_splitheader(struct http *hp, int req)
{
	char *p, *q, **hh;
	int n;
	char buf[20];

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	if (req) {
		memset(hp->req, 0, sizeof hp->req);
		hh = hp->req;
	} else {
		memset(hp->resp, 0, sizeof hp->resp);
		hh = hp->resp;
	}

	n = 0;
	p = hp->rxbuf;

	/* REQ/PROTO */
	while (vct_islws(*p))
		p++;
 	hh[n++] = p;
 	while (!vct_islws(*p))
 		p++;
	assert(!vct_iscrlf(p));
 	*p++ = '\0';
 
 	/* URL/STATUS */
 	while (vct_issp(*p))		/* XXX: H space only */
 		p++;
	assert(!vct_iscrlf(p));
 	hh[n++] = p;
 	while (!vct_islws(*p))
 		p++;
	if (vct_iscrlf(p)) {
 		hh[n++] = NULL;
 		q = p;
 		p += vct_skipcrlf(p);
		*q = '\0';
	} else {
		*p++ = '\0';
		/* PROTO/MSG */
 		while (vct_issp(*p))		/* XXX: H space only */
 			p++;
 		hh[n++] = p;
		while (!vct_iscrlf(p))
 			p++;
 		q = p;
 		p += vct_skipcrlf(p);
		*q = '\0';
	}
	assert(n == 3);
 
 	while (*p != '\0') {
 		assert(n < MAX_HDR);
		if (vct_iscrlf(p))
 			break;
 		hh[n++] = p++;
		while (*p != '\0' && !vct_iscrlf(p))
 			p++;
 		q = p;
 		p += vct_skipcrlf(p);
		*q = '\0';
	}
	p += vct_skipcrlf(p);
	assert(*p == '\0');

	for (n = 0; n < 3 || hh[n] != NULL; n++) {
		sprintf(buf, "http[%2d] ", n);
		vtc_dump(hp->vl, 4, buf, hh[n], -1);
	}
}
