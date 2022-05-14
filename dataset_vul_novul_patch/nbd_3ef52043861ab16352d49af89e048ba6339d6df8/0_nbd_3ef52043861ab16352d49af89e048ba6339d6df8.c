int mainloop(CLIENT *client) {
	struct nbd_request request;
	struct nbd_reply reply;
	gboolean go_on=TRUE;
#ifdef DODBG
	int i = 0;
#endif
	negotiate(client->net, client, NULL);
	DEBUG("Entering request loop!\n");
	reply.magic = htonl(NBD_REPLY_MAGIC);
	reply.error = 0;
	while (go_on) {
		char buf[BUFSIZE];
		size_t len;
#ifdef DODBG
		i++;
		printf("%d: ", i);
#endif
		readit(client->net, &request, sizeof(request));
		request.from = ntohll(request.from);
		request.type = ntohl(request.type);

		if (request.type==NBD_CMD_DISC) {
			msg2(LOG_INFO, "Disconnect request received.");
                	if (client->server->flags & F_COPYONWRITE) { 
				if (client->difmap) g_free(client->difmap) ;
                		close(client->difffile);
				unlink(client->difffilename);
				free(client->difffilename);
			}
			go_on=FALSE;
			continue;
		}

		len = ntohl(request.len);
 
 		if (request.magic != htonl(NBD_REQUEST_MAGIC))
 			err("Not enough magic.");
		if (len > BUFSIZE - sizeof(struct nbd_reply))
 			err("Request too big!");
 #ifdef DODBG
 		printf("%s from %llu (%llu) len %d, ", request.type ? "WRITE" :
				"READ", (unsigned long long)request.from,
				(unsigned long long)request.from / 512, len);
#endif
		memcpy(reply.handle, request.handle, sizeof(reply.handle));
		if ((request.from + len) > (OFFT_MAX)) {
			DEBUG("[Number too large!]");
			ERROR(client, reply, EINVAL);
			continue;
		}

		if (((ssize_t)((off_t)request.from + len) > client->exportsize)) {
			DEBUG("[RANGE!]");
			ERROR(client, reply, EINVAL);
			continue;
		}

		if (request.type==NBD_CMD_WRITE) {
			DEBUG("wr: net->buf, ");
			readit(client->net, buf, len);
			DEBUG("buf->exp, ");
			if ((client->server->flags & F_READONLY) ||
			    (client->server->flags & F_AUTOREADONLY)) {
				DEBUG("[WRITE to READONLY!]");
				ERROR(client, reply, EPERM);
				continue;
			}
			if (expwrite(request.from, buf, len, client)) {
				DEBUG("Write failed: %m" );
				ERROR(client, reply, errno);
				continue;
			}
			SEND(client->net, reply);
			DEBUG("OK!\n");
			continue;
		}
		/* READ */

		DEBUG("exp->buf, ");
		if (expread(request.from, buf + sizeof(struct nbd_reply), len, client)) {
			DEBUG("Read failed: %m");
			ERROR(client, reply, errno);
			continue;
		}

		DEBUG("buf->net, ");
		memcpy(buf, &reply, sizeof(struct nbd_reply));
		writeit(client->net, buf, len + sizeof(struct nbd_reply));
		DEBUG("OK!\n");
	}
	return 0;
}
