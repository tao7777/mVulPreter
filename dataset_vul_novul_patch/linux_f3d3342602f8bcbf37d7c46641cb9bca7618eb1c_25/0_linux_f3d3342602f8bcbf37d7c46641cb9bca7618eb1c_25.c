static int rawsock_recvmsg(struct kiocb *iocb, struct socket *sock,
			   struct msghdr *msg, size_t len, int flags)
{
	int noblock = flags & MSG_DONTWAIT;
	struct sock *sk = sock->sk;
	struct sk_buff *skb;
	int copied;
	int rc;

	pr_debug("sock=%p sk=%p len=%zu flags=%d\n", sock, sk, len, flags);

	skb = skb_recv_datagram(sk, flags, noblock, &rc);
 	if (!skb)
 		return rc;
 
 	copied = skb->len;
 	if (len < copied) {
 		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}

	rc = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, copied);

	skb_free_datagram(sk, skb);

	return rc ? : copied;
}
