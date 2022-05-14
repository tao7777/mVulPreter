static int ndp_sock_open(struct ndp *ndp)
{
	int sock;
	int ret;
	int err;
	int val;

	sock = socket(PF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
	if (sock == -1) {
		err(ndp, "Failed to create ICMP6 socket.");
		return -errno;
	}

	val = 1;
	ret = setsockopt(sock, IPPROTO_IPV6, IPV6_RECVPKTINFO,
			 &val, sizeof(val));
	if (ret == -1) {
		err(ndp, "Failed to setsockopt IPV6_RECVPKTINFO.");
		err = -errno;
		goto close_sock;
	}

	val = 255;
	ret = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
			 &val, sizeof(val));
	if (ret == -1) {
		err(ndp, "Failed to setsockopt IPV6_MULTICAST_HOPS.");
		err = -errno;
 		goto close_sock;
 	}
 
 	ndp->sock = sock;
 	return 0;
 close_sock:
	close(sock);
	return err;
}
