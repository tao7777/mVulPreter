int handle(int s, unsigned char* data, int len, struct sockaddr_in *s_in)
{
	char buf[2048];
	unsigned short *cmd = (unsigned short *)buf;
	int plen;
	struct in_addr *addr = &s_in->sin_addr;
	unsigned short *pid = (unsigned short*) data;

	/* inet check */
	if (len == S_HELLO_LEN && memcmp(data, "sorbo", 5) == 0) {
		unsigned short *id = (unsigned short*) (data+5);
		int x = 2+4+2;

		*cmd = htons(S_CMD_INET_CHECK);
		memcpy(cmd+1, addr, 4);
		memcpy(cmd+1+2, id, 2);

		printf("Inet check by %s %d\n",
		       inet_ntoa(*addr), ntohs(*id));
		if (send(s, buf, x, 0) != x)
			return 1;

		return 0;
	}

 	*cmd++ = htons(S_CMD_PACKET);
 	*cmd++ = *pid;
 	plen = len - 2;
    if (plen < 0)
        return 0;
 
 	last_id = ntohs(*pid);
 	if (last_id > 20000)
		wrap = 1;
	if (wrap && last_id < 100) {
		wrap = 0;
		memset(ids, 0, sizeof(ids));
	}

	printf("Got packet %d %d", last_id, plen);
	if (is_dup(last_id)) {
		printf(" (DUP)\n");
		return 0;
	}
	printf("\n");

	*cmd++ = htons(plen);
	memcpy(cmd, data+2, plen);

	plen += 2 + 2 + 2;
	assert(plen <= (int) sizeof(buf));
	if (send(s, buf, plen, 0) != plen)
		return 1;

	return 0;
}
