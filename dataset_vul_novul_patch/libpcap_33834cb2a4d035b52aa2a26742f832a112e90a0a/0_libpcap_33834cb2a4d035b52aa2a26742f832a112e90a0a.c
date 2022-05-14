daemon_msg_open_req(uint8 ver, struct daemon_slpars *pars, uint32 plen,
    char *source, size_t sourcelen)
{
	char errbuf[PCAP_ERRBUF_SIZE];		// buffer for network errors
	char errmsgbuf[PCAP_ERRBUF_SIZE];	// buffer for errors to send to the client
	pcap_t *fp;				// pcap_t main variable
	int nread;
	char sendbuf[RPCAP_NETBUF_SIZE];	// temporary buffer in which data to be sent is buffered
	int sendbufidx = 0;			// index which keeps the number of bytes currently buffered
	struct rpcap_openreply *openreply;	// open reply message

	if (plen > sourcelen - 1)
	{
		pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Source string too long");
		goto error;
	}

	nread = sock_recv(pars->sockctrl, source, plen,
	    SOCK_RECEIVEALL_YES|SOCK_EOF_IS_ERROR, errbuf, PCAP_ERRBUF_SIZE);
	if (nread == -1)
	{
		rpcapd_log(LOGPRIO_ERROR, "Read from client failed: %s", errbuf);
		return -1;
	}
 	source[nread] = '\0';
 	plen -= nread;
 
	// Is this a URL rather than a device?
	// If so, reject it.
	if (is_url(source))
	{
		pcap_snprintf(errmsgbuf, PCAP_ERRBUF_SIZE, "Source string refers to a remote device");
		goto error;
	}
 
	if ((fp = pcap_open_live(source,
			1500 /* fake snaplen */,
			0 /* no promis */,
			1000 /* fake timeout */,
			errmsgbuf)) == NULL)
		goto error;

	if (sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx,
	    RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
		goto error;

	rpcap_createhdr((struct rpcap_header *) sendbuf, ver,
	    RPCAP_MSG_OPEN_REPLY, 0, sizeof(struct rpcap_openreply));

	openreply = (struct rpcap_openreply *) &sendbuf[sendbufidx];

	if (sock_bufferize(NULL, sizeof(struct rpcap_openreply), NULL, &sendbufidx,
	    RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errmsgbuf, PCAP_ERRBUF_SIZE) == -1)
		goto error;

	memset(openreply, 0, sizeof(struct rpcap_openreply));
	openreply->linktype = htonl(pcap_datalink(fp));
	openreply->tzoff = 0; /* This is always 0 for live captures */

	pcap_close(fp);

	if (sock_send(pars->sockctrl, sendbuf, sendbufidx, errbuf, PCAP_ERRBUF_SIZE) == -1)
	{
		rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
		return -1;
	}
	return 0;

error:
	if (rpcap_senderror(pars->sockctrl, ver, PCAP_ERR_OPEN,
	    errmsgbuf, errbuf) == -1)
	{
		rpcapd_log(LOGPRIO_ERROR, "Send to client failed: %s", errbuf);
		return -1;
	}

	if (rpcapd_discard(pars->sockctrl, plen) == -1)
	{
		return -1;
	}
	return 0;
}
