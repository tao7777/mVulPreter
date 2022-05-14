static int hci_sock_recvmsg(struct kiocb *iocb, struct socket *sock,
			    struct msghdr *msg, size_t len, int flags)
{
	int noblock = flags & MSG_DONTWAIT;
	struct sock *sk = sock->sk;
	struct sk_buff *skb;
	int copied, err;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (flags & (MSG_OOB))
		return -EOPNOTSUPP;

	if (sk->sk_state == BT_CLOSED)
		return 0;

	skb = skb_recv_datagram(sk, flags, noblock, &err);
 	if (!skb)
 		return err;
 
 	copied = skb->len;
 	if (len < copied) {
 		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}

	skb_reset_transport_header(skb);
	err = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, copied);

	switch (hci_pi(sk)->channel) {
	case HCI_CHANNEL_RAW:
		hci_sock_cmsg(sk, msg, skb);
		break;
	case HCI_CHANNEL_USER:
	case HCI_CHANNEL_CONTROL:
	case HCI_CHANNEL_MONITOR:
		sock_recv_timestamp(msg, sk, skb);
		break;
	}

	skb_free_datagram(sk, skb);

	return err ? : copied;
}
