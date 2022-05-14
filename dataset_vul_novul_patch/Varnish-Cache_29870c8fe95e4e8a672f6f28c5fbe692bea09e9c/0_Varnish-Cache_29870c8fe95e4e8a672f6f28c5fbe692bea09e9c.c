htc_request_check_host_hdr(struct http *hp)
htc_request_check_hdrs(struct sess *sp, struct http *hp)
 {
 	int u;
 	int seen_host = 0;
	int seen_cl = 0;

 	for (u = HTTP_HDR_FIRST; u < hp->nhd; u++) {
 		if (hp->hd[u].b == NULL)
 			continue;
 		AN(hp->hd[u].b);
 		AN(hp->hd[u].e);
 		if (http_IsHdr(&hp->hd[u], H_Host)) {
 			if (seen_host) {
				WSP(sp, SLT_Error, "Duplicated Host header");
 				return (400);
 			}
 			seen_host = 1;
 		}
		if (http_IsHdr(&hp->hd[u], H_Content_Length)) {
			if (seen_cl) {
				WSP(sp, SLT_Error,
				    "Duplicated Content-Length header");
				return (400);
			}
			seen_cl = 1;
		}
 	}
 	return (0);
 }
