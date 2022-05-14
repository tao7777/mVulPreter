http_splitline(struct worker *w, int fd, struct http *hp,
    const struct http_conn *htc, int h1, int h2, int h3)
{
	char *p, *q;

	CHECK_OBJ_NOTNULL(htc, HTTP_CONN_MAGIC);
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	/* XXX: Assert a NUL at rx.e ? */
	Tcheck(htc->rxbuf);

	/* Skip leading LWS */
	for (p = htc->rxbuf.b ; vct_islws(*p); p++)
		continue;

	/* First field cannot contain SP, CRLF or CTL */
	q = p;
	for (; !vct_issp(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
	hp->hd[h1].b = q;
	hp->hd[h1].e = p;

	/* Skip SP */
	for (; vct_issp(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}

	/* Second field cannot contain LWS or CTL */
	q = p;
	for (; !vct_islws(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
	hp->hd[h2].b = q;
	hp->hd[h2].e = p;

	if (!Tlen(hp->hd[h2]))
		return (400);

	/* Skip SP */
	for (; vct_issp(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
 
 	/* Third field is optional and cannot contain CTL */
 	q = p;
	if (!vct_iscrlf(p)) {
		for (; !vct_iscrlf(p); p++)
 			if (!vct_issep(*p) && vct_isctl(*p))
 				return (400);
 	}
	hp->hd[h3].b = q;
	hp->hd[h3].e = p;

	/* Skip CRLF */
	p += vct_skipcrlf(p);

	*hp->hd[h1].e = '\0';
	WSLH(w, fd, hp, h1);

	*hp->hd[h2].e = '\0';
	WSLH(w, fd, hp, h2);

	if (hp->hd[h3].e != NULL) {
		*hp->hd[h3].e = '\0';
		WSLH(w, fd, hp, h3);
	}

	return (http_dissect_hdrs(w, hp, fd, p, htc));
}
