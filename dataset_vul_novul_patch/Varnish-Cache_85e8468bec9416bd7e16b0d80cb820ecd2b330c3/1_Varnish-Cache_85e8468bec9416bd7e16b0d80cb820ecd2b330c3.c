http_dissect_hdrs(struct worker *w, struct http *hp, int fd, char *p,
    const struct http_conn *htc)
{
	char *q, *r;
	txt t = htc->rxbuf;

	if (*p == '\r')
		p++;

	hp->nhd = HTTP_HDR_FIRST;
	hp->conds = 0;
	r = NULL;		/* For FlexeLint */
	for (; p < t.e; p = r) {

 		/* Find end of next header */
 		q = r = p;
 		while (r < t.e) {
			if (!vct_iscrlf(*r)) {
 				r++;
 				continue;
 			}
			q = r;
			assert(r < t.e);
			r += vct_skipcrlf(r);
			if (r >= t.e)
				break;
			/* If line does not continue: got it. */
			if (!vct_issp(*r))
				break;

			/* Clear line continuation LWS to spaces */
			while (vct_islws(*q))
				*q++ = ' ';
		}

		if (q - p > htc->maxhdr) {
			VSC_C_main->losthdr++;
			WSL(w, SLT_LostHeader, fd, "%.*s",
			    q - p > 20 ? 20 : q - p, p);
			return (413);
		}

		/* Empty header = end of headers */
		if (p == q)
			break;

		if ((p[0] == 'i' || p[0] == 'I') &&
		    (p[1] == 'f' || p[1] == 'F') &&
		    p[2] == '-')
			hp->conds = 1;

		while (q > p && vct_issp(q[-1]))
			q--;
		*q = '\0';

		if (hp->nhd < hp->shd) {
			hp->hdf[hp->nhd] = 0;
			hp->hd[hp->nhd].b = p;
			hp->hd[hp->nhd].e = q;
			WSLH(w, fd, hp, hp->nhd);
			hp->nhd++;
		} else {
			VSC_C_main->losthdr++;
			WSL(w, SLT_LostHeader, fd, "%.*s",
			    q - p > 20 ? 20 : q - p, p);
			return (413);
		}
	}
	return (0);
}
