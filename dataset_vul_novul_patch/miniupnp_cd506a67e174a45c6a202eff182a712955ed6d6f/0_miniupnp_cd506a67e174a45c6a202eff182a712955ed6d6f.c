updateDevice(const struct header * headers, time_t t)
{
	struct device ** pp = &devlist;
	struct device * p = *pp;	/* = devlist; */
	while(p)
	{
		if(  p->headers[HEADER_NT].l == headers[HEADER_NT].l
		  && (0==memcmp(p->headers[HEADER_NT].p, headers[HEADER_NT].p, headers[HEADER_NT].l))
		  && p->headers[HEADER_USN].l == headers[HEADER_USN].l
		  && (0==memcmp(p->headers[HEADER_USN].p, headers[HEADER_USN].p, headers[HEADER_USN].l)) )
		{
			/*printf("found! %d\n", (int)(t - p->t));*/
			syslog(LOG_DEBUG, "device updated : %.*s", headers[HEADER_USN].l, headers[HEADER_USN].p);
			p->t = t;
			/* update Location ! */
			if(headers[HEADER_LOCATION].l > p->headers[HEADER_LOCATION].l)
			{
				struct device * tmp;
				tmp = realloc(p, sizeof(struct device)
				    + headers[0].l+headers[1].l+headers[2].l);
 				if(!tmp)	/* allocation error */
 				{
 					syslog(LOG_ERR, "updateDevice() : memory allocation error");
					*pp = p->next;	/* remove "p" from the list */
 					free(p);
 					return 0;
 				}
				p = tmp;
				*pp = p;
			}
			memcpy(p->data + p->headers[0].l + p->headers[1].l,
			       headers[2].p, headers[2].l);
			/* TODO : check p->headers[HEADER_LOCATION].l */
			return 0;
		}
		pp = &p->next;
		p = *pp;	/* p = p->next; */
	}
	syslog(LOG_INFO, "new device discovered : %.*s",
	       headers[HEADER_USN].l, headers[HEADER_USN].p);
	/* add */
	{
		char * pc;
		int i;
		p = malloc(  sizeof(struct device)
		           + headers[0].l+headers[1].l+headers[2].l );
		if(!p) {
			syslog(LOG_ERR, "updateDevice(): cannot allocate memory");
			return -1;
		}
		p->next = devlist;
		p->t = t;
		pc = p->data;
		for(i = 0; i < 3; i++)
		{
			p->headers[i].p = pc;
			p->headers[i].l = headers[i].l;
			memcpy(pc, headers[i].p, headers[i].l);
			pc += headers[i].l;
		}
		devlist = p;
		sendNotifications(NOTIF_NEW, p, NULL);
	}
	return 1;
}
