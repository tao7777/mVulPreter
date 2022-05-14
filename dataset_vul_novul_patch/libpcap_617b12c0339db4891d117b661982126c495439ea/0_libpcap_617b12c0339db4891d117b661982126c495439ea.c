daemon_msg_findallif_req(uint8 ver, struct daemon_slpars *pars, uint32 plen)
{
	char errbuf[PCAP_ERRBUF_SIZE];		// buffer for network errors
	char errmsgbuf[PCAP_ERRBUF_SIZE];	// buffer for errors to send to the client
	char sendbuf[RPCAP_NETBUF_SIZE];	// temporary buffer in which data to be sent is buffered
	int sendbufidx = 0;			// index which keeps the number of bytes currently buffered
	pcap_if_t *alldevs = NULL;		// pointer to the header of the interface chain
 	pcap_if_t *d;				// temp pointer needed to scan the interface chain
 	struct pcap_addr *address;		// pcap structure that keeps a network address of an interface
 	struct rpcap_findalldevs_if *findalldevs_if;// rpcap structure that packet all the data of an interface together
	uint32 replylen;			// length of reply payload
 	uint16 nif = 0;				// counts the number of interface listed
 
	if (rpcapd_discard(pars->sockctrl, plen) == -1)
	{
		return -1;
	}

	if (pcap_findalldevs(&alldevs, errmsgbuf) == -1)
		goto error;

	if (alldevs == NULL)
	{
		if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_NOREMOTEIF,
			"No interfaces found! Make sure libpcap/WinPcap is properly installed"
			" and you have the right to access to the remote device.",
			errbuf) == -1)
		{
			rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
			return -1;
		}
 		return 0;
 	}
 
	// This checks the number of interfaces and computes the total
	// length of the payload.
	replylen = 0;
 	for (d = alldevs; d != NULL; d = d->next)
 	{
 		nif++;
 
 		if (d->description)
			replylen += strlen(d->description);
 		if (d->name)
			replylen += strlen(d->name);
 
		replylen += sizeof(struct rpcap_findalldevs_if);
 
 		for (address = d->addresses; address != NULL; address = address->next)
 		{
			/*
			 * Send only IPv4 and IPv6 addresses over the wire.
			 */
			switch (address->addr->sa_family)
			{
			case AF_INET:
 #ifdef AF_INET6
 			case AF_INET6:
 #endif
				replylen += (sizeof(struct rpcap_sockaddr) * 4);
 				break;
 
 			default:
				break;
			}
		}
	}

	if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL,
	    &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf,
	    PCAP_ERRBUF_SIZE) == -1)
 		goto error;
 
 	rpcap_createhdr((struct rpcap_header *) sendbuf, ver,
	    RPCAP_MSG_FINDALLIF_REPLY, nif, replylen);
 
 	for (d = alldevs; d != NULL; d = d->next)
	{
		uint16 lname, ldescr;

		findalldevs_if = (struct rpcap_findalldevs_if *) &sendbuf[sendbufidx];

		if (sock_bufferize(NULL, sizeof(struct rpcap_findalldevs_if), NULL,
		    &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
			goto error;

		memset(findalldevs_if, 0, sizeof(struct rpcap_findalldevs_if));

		if (d->description) ldescr = (short) strlen(d->description);
		else ldescr = 0;
		if (d->name) lname = (short) strlen(d->name);
		else lname = 0;

		findalldevs_if->desclen = htons(ldescr);
		findalldevs_if->namelen = htons(lname);
		findalldevs_if->flags = htonl(d->flags);

		for (address = d->addresses; address != NULL; address = address->next)
		{
			/*
			 * Send only IPv4 and IPv6 addresses over the wire.
			 */
			switch (address->addr->sa_family)
			{
			case AF_INET:
#ifdef AF_INET6
			case AF_INET6:
#endif
				findalldevs_if->naddr++;
				break;

			default:
				break;
			}
		}
		findalldevs_if->naddr = htons(findalldevs_if->naddr);

		if (sock_bufferize(d->name, lname, sendbuf, &sendbufidx,
		    RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errmsgbuf,
		    PCAP_ERRBUF_SIZE) == -1)
			goto error;

		if (sock_bufferize(d->description, ldescr, sendbuf, &sendbufidx,
		    RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errmsgbuf,
		    PCAP_ERRBUF_SIZE) == -1)
			goto error;

		for (address = d->addresses; address != NULL; address = address->next)
		{
			struct rpcap_sockaddr *sockaddr;

			/*
			 * Send only IPv4 and IPv6 addresses over the wire.
			 */
			switch (address->addr->sa_family)
			{
			case AF_INET:
#ifdef AF_INET6
			case AF_INET6:
#endif
				sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
				if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
				    &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
					goto error;
				daemon_seraddr((struct sockaddr_storage *) address->addr, sockaddr);

				sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
				if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
				    &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
					goto error;
				daemon_seraddr((struct sockaddr_storage *) address->netmask, sockaddr);

				sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
				if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
				    &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
					goto error;
				daemon_seraddr((struct sockaddr_storage *) address->broadaddr, sockaddr);

				sockaddr = (struct rpcap_sockaddr *) &sendbuf[sendbufidx];
				if (sock_bufferize(NULL, sizeof(struct rpcap_sockaddr), NULL,
				    &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
					goto error;
				daemon_seraddr((struct sockaddr_storage *) address->dstaddr, sockaddr);
				break;

			default:
				break;
			}
		}
	}

	pcap_freealldevs(alldevs);

	if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
	{
		rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
		return -1;
	}

	return 0;

error:
	if (alldevs)
		pcap_freealldevs(alldevs);

	if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_FINDALLIF,
	    errmsgbuf, errbuf) == -1)
	{
		rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
		return -1;
	}
	return 0;
}
