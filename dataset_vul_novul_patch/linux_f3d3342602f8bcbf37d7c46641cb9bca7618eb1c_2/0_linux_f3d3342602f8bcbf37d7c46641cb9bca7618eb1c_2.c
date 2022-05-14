static int pppoe_recvmsg(struct kiocb *iocb, struct socket *sock,
		  struct msghdr *m, size_t total_len, int flags)
{
	struct sock *sk = sock->sk;
	struct sk_buff *skb;
	int error = 0;

	if (sk->sk_state & PPPOX_BOUND) {
		error = -EIO;
		goto end;
	}

	skb = skb_recv_datagram(sk, flags & ~MSG_DONTWAIT,
				flags & MSG_DONTWAIT, &error);
 	if (error < 0)
 		goto end;
 
 	if (skb) {
 		total_len = min_t(size_t, total_len, skb->len);
 		error = skb_copy_datagram_iovec(skb, 0, m->msg_iov, total_len);
		if (error == 0) {
			consume_skb(skb);
			return total_len;
		}
	}

	kfree_skb(skb);
end:
	return error;
}
