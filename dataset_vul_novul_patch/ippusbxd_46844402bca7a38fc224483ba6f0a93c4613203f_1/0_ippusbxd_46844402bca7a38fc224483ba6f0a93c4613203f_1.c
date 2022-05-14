struct tcp_sock_t *tcp_open(uint16_t port)
 {
 	struct tcp_sock_t *this = calloc(1, sizeof *this);
 	if (this == NULL) {
		ERR("IPv4: callocing this failed");
		goto error;
	}

	// Open [S]ocket [D]escriptor
	this->sd = -1;
	this->sd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sd < 0) {
		ERR("IPv4 socket open failed");
		goto error;
	}

	// Configure socket params
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(0x7F000001);

	// Bind to localhost
	if (bind(this->sd,
	        (struct sockaddr *)&addr,
	        sizeof addr) < 0) {
		if (g_options.only_desired_port == 1)
			ERR("IPv4 bind on port failed. "
			    "Requested port may be taken or require root permissions.");
		goto error;
	}

	// Let kernel over-accept max number of connections
	if (listen(this->sd, HTTP_MAX_PENDING_CONNS) < 0) {
		ERR("IPv4 listen failed on socket");
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

struct tcp_sock_t *tcp6_open(uint16_t port)
{
	struct tcp_sock_t *this = calloc(1, sizeof *this);
	if (this == NULL) {
		ERR("IPv6: callocing this failed");
 		goto error;
 	}
 
 	this->sd = -1;
 	this->sd = socket(AF_INET6, SOCK_STREAM, 0);
 	if (this->sd < 0) {
		ERR("Ipv6 socket open failed");
 		goto error;
 	}
 
	struct sockaddr_in6 addr;
 	memset(&addr, 0, sizeof addr);
 	addr.sin6_family = AF_INET6;
 	addr.sin6_port = htons(port);
	addr.sin6_addr = in6addr_loopback;
 
 	if (bind(this->sd,
 	        (struct sockaddr *)&addr,
 	        sizeof addr) < 0) {
 		if (g_options.only_desired_port == 1)
			ERR("IPv6 bind on port failed. "
 			    "Requested port may be taken or require root permissions.");
 		goto error;
 	}
 
 	if (listen(this->sd, HTTP_MAX_PENDING_CONNS) < 0) {
		ERR("IPv6 listen failed on socket");
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
