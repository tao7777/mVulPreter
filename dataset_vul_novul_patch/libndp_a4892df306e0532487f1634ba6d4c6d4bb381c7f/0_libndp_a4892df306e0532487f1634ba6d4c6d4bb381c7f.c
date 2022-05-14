 static int myrecvfrom6(int sockfd, void *buf, size_t *buflen, int flags,
		       struct in6_addr *addr, uint32_t *ifindex, int *hoplimit)
 {
 	struct sockaddr_in6 sin6;
	unsigned char cbuf[2 * CMSG_SPACE(sizeof(struct in6_pktinfo))];
 	struct iovec iovec;
 	struct msghdr msghdr;
 	struct cmsghdr *cmsghdr;
	ssize_t len;

	iovec.iov_len = *buflen;
	iovec.iov_base = buf;
	memset(&msghdr, 0, sizeof(msghdr));
	msghdr.msg_name = &sin6;
	msghdr.msg_namelen = sizeof(sin6);
	msghdr.msg_iov = &iovec;
	msghdr.msg_iovlen = 1;
	msghdr.msg_control = cbuf;
	msghdr.msg_controllen = sizeof(cbuf);

	len = recvmsg(sockfd, &msghdr, flags);
	if (len == -1)
		return -errno;
	*buflen = len;

	/* Set ifindex to scope_id now. But since scope_id gets not
	 * set by kernel for linklocal addresses, use pktinfo to obtain that
	 * value right after.
	 */
 	*ifindex = sin6.sin6_scope_id;
         for (cmsghdr = CMSG_FIRSTHDR(&msghdr); cmsghdr;
 	     cmsghdr = CMSG_NXTHDR(&msghdr, cmsghdr)) {
		if (cmsghdr->cmsg_level != IPPROTO_IPV6)
			continue;

		switch(cmsghdr->cmsg_type) {
		case IPV6_PKTINFO:
			if (cmsghdr->cmsg_len == CMSG_LEN(sizeof(struct in6_pktinfo))) {
				struct in6_pktinfo *pktinfo;

				pktinfo = (struct in6_pktinfo *) CMSG_DATA(cmsghdr);
				*ifindex = pktinfo->ipi6_ifindex;
			}
			break;
		case IPV6_HOPLIMIT:
			if (cmsghdr->cmsg_len == CMSG_LEN(sizeof(int))) {
				int *val;
 
				val = (int *) CMSG_DATA(cmsghdr);
				*hoplimit = *val;
			}
			break;
 		}
 	}
 	*addr = sin6.sin6_addr;

	return 0;
}
