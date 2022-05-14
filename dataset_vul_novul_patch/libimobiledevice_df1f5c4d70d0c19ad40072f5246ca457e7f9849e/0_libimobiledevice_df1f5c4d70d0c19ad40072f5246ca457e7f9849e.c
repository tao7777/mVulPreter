int socket_accept(int fd, uint16_t port)
{
#ifdef WIN32
	int addr_len;
#else
	socklen_t addr_len;
#endif
	int result;
	struct sockaddr_in addr;
 
 	memset(&addr, 0, sizeof(addr));
 	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
 	addr.sin_port = htons(port);
 
 	addr_len = sizeof(addr);
	result = accept(fd, (struct sockaddr*)&addr, &addr_len);

	return result;
}
