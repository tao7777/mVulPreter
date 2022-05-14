static int pfkey_recvmsg(struct kiocb *kiocb,
			 struct socket *sock, struct msghdr *msg, size_t len,
			 int flags)
{
	struct sock *sk = sock->sk;
	struct pfkey_sock *pfk = pfkey_sk(sk);
	struct sk_buff *skb;
	int copied, err;

	err = -EINVAL;
 	if (flags & ~(MSG_PEEK|MSG_DONTWAIT|MSG_TRUNC|MSG_CMSG_COMPAT))
 		goto out;
 
 	skb = skb_recv_datagram(sk, flags, flags & MSG_DONTWAIT, &err);
 	if (skb == NULL)
 		goto out;

	copied = skb->len;
	if (copied > len) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}

	skb_reset_transport_header(skb);
	err = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, copied);
	if (err)
		goto out_free;

	sock_recv_ts_and_drops(msg, sk, skb);

	err = (flags & MSG_TRUNC) ? skb->len : copied;

	if (pfk->dump.dump != NULL &&
	    3 * atomic_read(&sk->sk_rmem_alloc) <= sk->sk_rcvbuf)
		pfkey_do_dump(pfk);

out_free:
	skb_free_datagram(sk, skb);
out:
	return err;
}
