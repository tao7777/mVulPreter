htc_request_check_host_hdr(struct http *hp)
 {
 	int u;
 	int seen_host = 0;
 	for (u = HTTP_HDR_FIRST; u < hp->nhd; u++) {
 		if (hp->hd[u].b == NULL)
 			continue;
 		AN(hp->hd[u].b);
 		AN(hp->hd[u].e);
 		if (http_IsHdr(&hp->hd[u], H_Host)) {
 			if (seen_host) {
 				return (400);
 			}
 			seen_host = 1;
 		}
 	}
 	return (0);
 }
