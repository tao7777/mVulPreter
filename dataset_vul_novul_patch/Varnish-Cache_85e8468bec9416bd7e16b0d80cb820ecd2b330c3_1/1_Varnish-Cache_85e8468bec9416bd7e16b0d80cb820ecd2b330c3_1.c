http_rxchunk(struct http *hp)
{
	char *q;
	int l, i;

	l = hp->prxbuf;
	do
		(void)http_rxchar(hp, 1, 0);
	while (hp->rxbuf[hp->prxbuf - 1] != '\n');
	vtc_dump(hp->vl, 4, "len", hp->rxbuf + l, -1);
	i = strtoul(hp->rxbuf + l, &q, 16);
	bprintf(hp->chunklen, "%d", i);
	if ((q == hp->rxbuf + l) ||
		(*q != '\0' && !vct_islws(*q))) {
		vtc_log(hp->vl, hp->fatal, "chunked fail %02x @ %d",
		    *q, q - (hp->rxbuf + l));
	}
	assert(q != hp->rxbuf + l);
	assert(*q == '\0' || vct_islws(*q));
	hp->prxbuf = l;
	if (i > 0) {
		(void)http_rxchar(hp, i, 0);
		vtc_dump(hp->vl, 4, "chunk",
		    hp->rxbuf + l, i);
 	}
 	l = hp->prxbuf;
 	(void)http_rxchar(hp, 2, 0);
	if(!vct_iscrlf(hp->rxbuf[l]))
 		vtc_log(hp->vl, hp->fatal,
 		    "Wrong chunk tail[0] = %02x",
 		    hp->rxbuf[l] & 0xff);
	if(!vct_iscrlf(hp->rxbuf[l + 1]))
 		vtc_log(hp->vl, hp->fatal,
 		    "Wrong chunk tail[1] = %02x",
 		    hp->rxbuf[l + 1] & 0xff);
	hp->prxbuf = l;
	hp->rxbuf[l] = '\0';
	return (i);
}
