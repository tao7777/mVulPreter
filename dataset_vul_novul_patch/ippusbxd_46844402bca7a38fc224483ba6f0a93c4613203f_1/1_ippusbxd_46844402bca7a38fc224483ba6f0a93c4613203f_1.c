struct tcp_sock_t *tcp_open(uint16_t port)
 {
 	struct tcp_sock_t *this = calloc(1, sizeof *this);
 	if (this == NULL) {
		ERR("callocing this failed");
 		goto error;
 	}
 
 	this->sd = -1;
 	this->sd = socket(AF_INET6, SOCK_STREAM, 0);
 	if (this->sd < 0) {
		ERR("sockect open failed");
 		goto error;
 	}
 
	struct sockaddr_in6 addr;
 	memset(&addr, 0, sizeof addr);
 	addr.sin6_family = AF_INET6;
 	addr.sin6_port = htons(port);
	addr.sin6_addr = in6addr_any;
 
 	if (bind(this->sd,
 	        (struct sockaddr *)&addr,
 	        sizeof addr) < 0) {
 		if (g_options.only_desired_port == 1)
			ERR("Bind on port failed. "
 			    "Requested port may be taken or require root permissions.");
 		goto error;
 	}
 
 	if (listen(this->sd, HTTP_MAX_PENDING_CONNS) < 0) {
		ERR("listen failed on socket");
 		goto error;
 	}
 
	return this;

error:
	if (this != NULL) {
		if (this->sd != -1) {
			close(this->sd);
		}
		free(this);
	}
	return NULL;
}
