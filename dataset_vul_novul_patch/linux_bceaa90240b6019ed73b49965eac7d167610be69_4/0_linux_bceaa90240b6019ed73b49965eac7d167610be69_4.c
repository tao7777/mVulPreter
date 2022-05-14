int udpv6_recvmsg(struct kiocb *iocb, struct sock *sk,
		  struct msghdr *msg, size_t len,
		  int noblock, int flags, int *addr_len)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct inet_sock *inet = inet_sk(sk);
	struct sk_buff *skb;
	unsigned int ulen, copied;
	int peeked, off = 0;
	int err;
	int is_udplite = IS_UDPLITE(sk);
 	int is_udp4;
 	bool slow;
 
 	if (flags & MSG_ERRQUEUE)
 		return ipv6_recv_error(sk, msg, len);
 
	if (np->rxpmtu && np->rxopt.bits.rxpmtu)
		return ipv6_recv_rxpmtu(sk, msg, len);

try_again:
	skb = __skb_recv_datagram(sk, flags | (noblock ? MSG_DONTWAIT : 0),
				  &peeked, &off, &err);
	if (!skb)
		goto out;

	ulen = skb->len - sizeof(struct udphdr);
	copied = len;
	if (copied > ulen)
		copied = ulen;
	else if (copied < ulen)
		msg->msg_flags |= MSG_TRUNC;

	is_udp4 = (skb->protocol == htons(ETH_P_IP));

	/*
	 * If checksum is needed at all, try to do it while copying the
	 * data.  If the data is truncated, or if we only want a partial
	 * coverage checksum (UDP-Lite), do it before the copy.
	 */

	if (copied < ulen || UDP_SKB_CB(skb)->partial_cov) {
		if (udp_lib_checksum_complete(skb))
			goto csum_copy_err;
	}

	if (skb_csum_unnecessary(skb))
		err = skb_copy_datagram_iovec(skb, sizeof(struct udphdr),
					      msg->msg_iov, copied);
	else {
		err = skb_copy_and_csum_datagram_iovec(skb, sizeof(struct udphdr), msg->msg_iov);
		if (err == -EINVAL)
			goto csum_copy_err;
	}
	if (unlikely(err)) {
		trace_kfree_skb(skb, udpv6_recvmsg);
		if (!peeked) {
			atomic_inc(&sk->sk_drops);
			if (is_udp4)
				UDP_INC_STATS_USER(sock_net(sk),
						   UDP_MIB_INERRORS,
						   is_udplite);
			else
				UDP6_INC_STATS_USER(sock_net(sk),
						    UDP_MIB_INERRORS,
						    is_udplite);
		}
		goto out_free;
	}
	if (!peeked) {
		if (is_udp4)
			UDP_INC_STATS_USER(sock_net(sk),
					UDP_MIB_INDATAGRAMS, is_udplite);
		else
			UDP6_INC_STATS_USER(sock_net(sk),
					UDP_MIB_INDATAGRAMS, is_udplite);
	}

	sock_recv_ts_and_drops(msg, sk, skb);

	/* Copy the address. */
	if (msg->msg_name) {
		struct sockaddr_in6 *sin6;

		sin6 = (struct sockaddr_in6 *) msg->msg_name;
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = udp_hdr(skb)->source;
		sin6->sin6_flowinfo = 0;

		if (is_udp4) {
			ipv6_addr_set_v4mapped(ip_hdr(skb)->saddr,
					       &sin6->sin6_addr);
			sin6->sin6_scope_id = 0;
		} else {
			sin6->sin6_addr = ipv6_hdr(skb)->saddr;
			sin6->sin6_scope_id =
 				ipv6_iface_scope_id(&sin6->sin6_addr,
 						    IP6CB(skb)->iif);
 		}
		*addr_len = sizeof(*sin6);
 	}
 	if (is_udp4) {
 		if (inet->cmsg_flags)
			ip_cmsg_recv(msg, skb);
	} else {
		if (np->rxopt.all)
			ip6_datagram_recv_ctl(sk, msg, skb);
	}

	err = copied;
	if (flags & MSG_TRUNC)
		err = ulen;

out_free:
	skb_free_datagram_locked(sk, skb);
out:
	return err;

csum_copy_err:
	slow = lock_sock_fast(sk);
	if (!skb_kill_datagram(sk, skb, flags)) {
		if (is_udp4) {
			UDP_INC_STATS_USER(sock_net(sk),
					UDP_MIB_CSUMERRORS, is_udplite);
			UDP_INC_STATS_USER(sock_net(sk),
					UDP_MIB_INERRORS, is_udplite);
		} else {
			UDP6_INC_STATS_USER(sock_net(sk),
					UDP_MIB_CSUMERRORS, is_udplite);
			UDP6_INC_STATS_USER(sock_net(sk),
					UDP_MIB_INERRORS, is_udplite);
		}
	}
	unlock_sock_fast(sk, slow);

	if (noblock)
		return -EAGAIN;

	/* starting over for a new packet */
	msg->msg_flags &= ~MSG_TRUNC;
	goto try_again;
}
