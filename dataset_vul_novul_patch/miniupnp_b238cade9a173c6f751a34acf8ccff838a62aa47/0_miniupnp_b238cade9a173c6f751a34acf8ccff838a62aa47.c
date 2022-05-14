void processRequest(struct reqelem * req)
{
	ssize_t n;
	unsigned int l, m;
	unsigned char buf[2048];
	const unsigned char * p;
	enum request_type type;
	struct device * d = devlist;
	unsigned char rbuf[RESPONSE_BUFFER_SIZE];
	unsigned char * rp;
	unsigned char nrep = 0;
	time_t t;
	struct service * newserv = NULL;
	struct service * serv;

	n = read(req->socket, buf, sizeof(buf));
	if(n<0) {
		if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
			return;	/* try again later */
		syslog(LOG_ERR, "(s=%d) processRequest(): read(): %m", req->socket);
		goto error;
	}
	if(n==0) {
		syslog(LOG_INFO, "(s=%d) request connection closed", req->socket);
		goto error;
	}
	t = time(NULL);
 	type = buf[0];
 	p = buf + 1;
 	DECODELENGTH_CHECKLIMIT(l, p, buf + n);
	if(l > (unsigned)(buf+n-p)) {
 		syslog(LOG_WARNING, "bad request (length encoding l=%u n=%u)",
 		       l, (unsigned)n);
 		goto error;
	}
	if(l == 0 && type != MINISSDPD_SEARCH_ALL
	   && type != MINISSDPD_GET_VERSION && type != MINISSDPD_NOTIF) {
		syslog(LOG_WARNING, "bad request (length=0, type=%d)", type);
		goto error;
	}
	syslog(LOG_INFO, "(s=%d) request type=%d str='%.*s'",
	       req->socket, type, l, p);
	switch(type) {
	case MINISSDPD_GET_VERSION:
		rp = rbuf;
		CODELENGTH((sizeof(MINISSDPD_VERSION) - 1), rp);
		memcpy(rp, MINISSDPD_VERSION, sizeof(MINISSDPD_VERSION) - 1);
		rp += (sizeof(MINISSDPD_VERSION) - 1);
		if(write_or_buffer(req, rbuf, rp - rbuf) < 0) {
			syslog(LOG_ERR, "(s=%d) write: %m", req->socket);
			goto error;
		}
		break;
	case MINISSDPD_SEARCH_TYPE:	/* request by type */
	case MINISSDPD_SEARCH_USN:	/* request by USN (unique id) */
	case MINISSDPD_SEARCH_ALL:	/* everything */
		rp = rbuf+1;
		while(d && (nrep < 255)) {
			if(d->t < t) {
				syslog(LOG_INFO, "outdated device");
			} else {
				/* test if we can put more responses in the buffer */
				if(d->headers[HEADER_LOCATION].l + d->headers[HEADER_NT].l
				  + d->headers[HEADER_USN].l + 6
				  + (rp - rbuf) >= (int)sizeof(rbuf))
					break;
				if( (type==MINISSDPD_SEARCH_TYPE && 0==memcmp(d->headers[HEADER_NT].p, p, l))
				  ||(type==MINISSDPD_SEARCH_USN && 0==memcmp(d->headers[HEADER_USN].p, p, l))
				  ||(type==MINISSDPD_SEARCH_ALL) ) {
					/* response :
					 * 1 - Location
					 * 2 - NT (device/service type)
					 * 3 - usn */
					m = d->headers[HEADER_LOCATION].l;
					CODELENGTH(m, rp);
					memcpy(rp, d->headers[HEADER_LOCATION].p, d->headers[HEADER_LOCATION].l);
					rp += d->headers[HEADER_LOCATION].l;
					m = d->headers[HEADER_NT].l;
					CODELENGTH(m, rp);
					memcpy(rp, d->headers[HEADER_NT].p, d->headers[HEADER_NT].l);
					rp += d->headers[HEADER_NT].l;
					m = d->headers[HEADER_USN].l;
					CODELENGTH(m, rp);
					memcpy(rp, d->headers[HEADER_USN].p, d->headers[HEADER_USN].l);
					rp += d->headers[HEADER_USN].l;
					nrep++;
				}
			}
			d = d->next;
		}
		/* Also look in service list */
		for(serv = servicelisthead.lh_first;
		    serv && (nrep < 255);
		    serv = serv->entries.le_next) {
			/* test if we can put more responses in the buffer */
			if(strlen(serv->location) + strlen(serv->st)
			  + strlen(serv->usn) + 6 + (rp - rbuf) >= sizeof(rbuf))
			  	break;
			if( (type==MINISSDPD_SEARCH_TYPE && 0==strncmp(serv->st, (const char *)p, l))
			  ||(type==MINISSDPD_SEARCH_USN && 0==strncmp(serv->usn, (const char *)p, l))
			  ||(type==MINISSDPD_SEARCH_ALL) ) {
				/* response :
				 * 1 - Location
				 * 2 - NT (device/service type)
				 * 3 - usn */
				m = strlen(serv->location);
				CODELENGTH(m, rp);
				memcpy(rp, serv->location, m);
				rp += m;
				m = strlen(serv->st);
				CODELENGTH(m, rp);
				memcpy(rp, serv->st, m);
				rp += m;
				m = strlen(serv->usn);
				CODELENGTH(m, rp);
				memcpy(rp, serv->usn, m);
				rp += m;
				nrep++;
			}
		}
		rbuf[0] = nrep;
		syslog(LOG_DEBUG, "(s=%d) response : %d device%s",
		       req->socket, nrep, (nrep > 1) ? "s" : "");
		if(write_or_buffer(req, rbuf, rp - rbuf) < 0) {
			syslog(LOG_ERR, "(s=%d) write: %m", req->socket);
			goto error;
		}
		break;
	case MINISSDPD_SUBMIT:	/* submit service */
		newserv = malloc(sizeof(struct service));
		if(!newserv) {
			syslog(LOG_ERR, "cannot allocate memory");
			goto error;
		}
		memset(newserv, 0, sizeof(struct service));	/* set pointers to NULL */
		if(containsForbiddenChars(p, l)) {
			syslog(LOG_ERR, "bad request (st contains forbidden chars)");
			goto error;
		}
		newserv->st = malloc(l + 1);
		if(!newserv->st) {
			syslog(LOG_ERR, "cannot allocate memory");
			goto error;
		}
		memcpy(newserv->st, p, l);
		newserv->st[l] = '\0';
		p += l;
		if(p >= buf + n) {
			syslog(LOG_WARNING, "bad request (missing usn)");
 			goto error;
 		}
 		DECODELENGTH_CHECKLIMIT(l, p, buf + n);
		if(l > (unsigned)(buf+n-p)) {
 			syslog(LOG_WARNING, "bad request (length encoding)");
 			goto error;
 		}
		if(containsForbiddenChars(p, l)) {
			syslog(LOG_ERR, "bad request (usn contains forbidden chars)");
			goto error;
		}
		syslog(LOG_INFO, "usn='%.*s'", l, p);
		newserv->usn = malloc(l + 1);
		if(!newserv->usn) {
			syslog(LOG_ERR, "cannot allocate memory");
			goto error;
		}
		memcpy(newserv->usn, p, l);
 		newserv->usn[l] = '\0';
 		p += l;
 		DECODELENGTH_CHECKLIMIT(l, p, buf + n);
		if(l > (unsigned)(buf+n-p)) {
 			syslog(LOG_WARNING, "bad request (length encoding)");
 			goto error;
 		}
		if(containsForbiddenChars(p, l)) {
			syslog(LOG_ERR, "bad request (server contains forbidden chars)");
			goto error;
		}
		syslog(LOG_INFO, "server='%.*s'", l, p);
		newserv->server = malloc(l + 1);
		if(!newserv->server) {
			syslog(LOG_ERR, "cannot allocate memory");
			goto error;
		}
		memcpy(newserv->server, p, l);
 		newserv->server[l] = '\0';
 		p += l;
 		DECODELENGTH_CHECKLIMIT(l, p, buf + n);
		if(l > (unsigned)(buf+n-p)) {
 			syslog(LOG_WARNING, "bad request (length encoding)");
 			goto error;
 		}
		if(containsForbiddenChars(p, l)) {
			syslog(LOG_ERR, "bad request (location contains forbidden chars)");
			goto error;
		}
		syslog(LOG_INFO, "location='%.*s'", l, p);
		newserv->location = malloc(l + 1);
		if(!newserv->location) {
			syslog(LOG_ERR, "cannot allocate memory");
			goto error;
		}
		memcpy(newserv->location, p, l);
		newserv->location[l] = '\0';
		/* look in service list for duplicate */
		for(serv = servicelisthead.lh_first;
		    serv;
		    serv = serv->entries.le_next) {
			if(0 == strcmp(newserv->usn, serv->usn)
			  && 0 == strcmp(newserv->st, serv->st)) {
				syslog(LOG_INFO, "Service already in the list. Updating...");
				free(newserv->st);
				free(newserv->usn);
				free(serv->server);
				serv->server = newserv->server;
				free(serv->location);
				serv->location = newserv->location;
				free(newserv);
				newserv = NULL;
				return;
			}
		}
		/* Inserting new service */
		LIST_INSERT_HEAD(&servicelisthead, newserv, entries);
		sendNotifications(NOTIF_NEW, NULL, newserv);
		newserv = NULL;
		break;
	case MINISSDPD_NOTIF:	/* switch socket to notify */
		rbuf[0] = '\0';
		if(write_or_buffer(req, rbuf, 1) < 0) {
			syslog(LOG_ERR, "(s=%d) write: %m", req->socket);
			goto error;
		}
		req->is_notify = 1;
		break;
	default:
		syslog(LOG_WARNING, "Unknown request type %d", type);
		rbuf[0] = '\0';
		if(write_or_buffer(req, rbuf, 1) < 0) {
			syslog(LOG_ERR, "(s=%d) write: %m", req->socket);
			goto error;
		}
	}
	return;
error:
	if(newserv) {
		free(newserv->st);
		free(newserv->usn);
		free(newserv->server);
		free(newserv->location);
		free(newserv);
		newserv = NULL;
	}
	close(req->socket);
	req->socket = -1;
	return;
}
