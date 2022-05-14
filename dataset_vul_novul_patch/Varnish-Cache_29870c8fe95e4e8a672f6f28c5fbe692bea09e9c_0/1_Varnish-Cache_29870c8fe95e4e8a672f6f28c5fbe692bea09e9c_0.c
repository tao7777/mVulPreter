http_DissectRequest(struct sess *sp)
{
	struct http_conn *htc;
	struct http *hp;
	uint16_t retval;

	CHECK_OBJ_NOTNULL(sp, SESS_MAGIC);
	htc = sp->htc;
	CHECK_OBJ_NOTNULL(htc, HTTP_CONN_MAGIC);
	hp = sp->http;
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	hp->logtag = HTTP_Rx;

	retval = http_splitline(sp->wrk, sp->fd, hp, htc,
	    HTTP_HDR_REQ, HTTP_HDR_URL, HTTP_HDR_PROTO);
	if (retval != 0) {
		WSPR(sp, SLT_HttpGarbage, htc->rxbuf);
		return (retval);
 	}
 	http_ProtoVer(hp);
 
	retval = htc_request_check_host_hdr(hp);
	if (retval != 0) {
		WSP(sp, SLT_Error, "Duplicated Host header");
		return (retval);
	}
 	return (retval);
 }
