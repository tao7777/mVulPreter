static int ng_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_ng *pkt;
	const char *ptr;
	size_t alloclen;

	pkt = git__malloc(sizeof(*pkt));
	GITERR_CHECK_ALLOC(pkt);

 	pkt->ref = NULL;
 	pkt->type = GIT_PKT_NG;
 
	if (len < 3)
		goto out_err;
 	line += 3; /* skip "ng " */
	len -= 3;
	if (!(ptr = memchr(line, ' ', len)))
 		goto out_err;
 	len = ptr - line;
 
	GITERR_CHECK_ALLOC_ADD(&alloclen, len, 1);
	pkt->ref = git__malloc(alloclen);
	GITERR_CHECK_ALLOC(pkt->ref);

 	memcpy(pkt->ref, line, len);
 	pkt->ref[len] = '\0';
 
	if (len < 1)
		goto out_err;
 	line = ptr + 1;
	len -= 1;
	if (!(ptr = memchr(line, '\n', len)))
 		goto out_err;
 	len = ptr - line;
 
	GITERR_CHECK_ALLOC_ADD(&alloclen, len, 1);
	pkt->msg = git__malloc(alloclen);
	GITERR_CHECK_ALLOC(pkt->msg);

	memcpy(pkt->msg, line, len);
	pkt->msg[len] = '\0';

	*out = (git_pkt *)pkt;
	return 0;

out_err:
	giterr_set(GITERR_NET, "invalid packet line");
	git__free(pkt->ref);
	git__free(pkt);
	return -1;
}
