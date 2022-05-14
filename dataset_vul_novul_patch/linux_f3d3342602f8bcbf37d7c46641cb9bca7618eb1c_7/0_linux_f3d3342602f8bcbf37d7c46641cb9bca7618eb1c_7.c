int bt_sock_stream_recvmsg(struct kiocb *iocb, struct socket *sock,
			       struct msghdr *msg, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	int err = 0;
	size_t target, copied = 0;
	long timeo;

 	if (flags & MSG_OOB)
 		return -EOPNOTSUPP;
 
 	BT_DBG("sk %p size %zu", sk, size);
 
 	lock_sock(sk);

	target = sock_rcvlowat(sk, flags & MSG_WAITALL, size);
	timeo  = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);

	do {
		struct sk_buff *skb;
		int chunk;

		skb = skb_dequeue(&sk->sk_receive_queue);
		if (!skb) {
			if (copied >= target)
				break;

			err = sock_error(sk);
			if (err)
				break;
			if (sk->sk_shutdown & RCV_SHUTDOWN)
				break;

			err = -EAGAIN;
			if (!timeo)
				break;

			timeo = bt_sock_data_wait(sk, timeo);

			if (signal_pending(current)) {
				err = sock_intr_errno(timeo);
				goto out;
			}
			continue;
		}

		chunk = min_t(unsigned int, skb->len, size);
		if (skb_copy_datagram_iovec(skb, 0, msg->msg_iov, chunk)) {
			skb_queue_head(&sk->sk_receive_queue, skb);
			if (!copied)
				copied = -EFAULT;
			break;
		}
		copied += chunk;
		size   -= chunk;

		sock_recv_ts_and_drops(msg, sk, skb);

		if (!(flags & MSG_PEEK)) {
			int skb_len = skb_headlen(skb);

			if (chunk <= skb_len) {
				__skb_pull(skb, chunk);
			} else {
				struct sk_buff *frag;

				__skb_pull(skb, skb_len);
				chunk -= skb_len;

				skb_walk_frags(skb, frag) {
					if (chunk <= frag->len) {
						/* Pulling partial data */
						skb->len -= chunk;
						skb->data_len -= chunk;
						__skb_pull(frag, chunk);
						break;
					} else if (frag->len) {
						/* Pulling all frag data */
						chunk -= frag->len;
						skb->len -= frag->len;
						skb->data_len -= frag->len;
						__skb_pull(frag, frag->len);
					}
				}
			}

			if (skb->len) {
				skb_queue_head(&sk->sk_receive_queue, skb);
				break;
			}
			kfree_skb(skb);

		} else {
			/* put message back and return */
			skb_queue_head(&sk->sk_receive_queue, skb);
			break;
		}
	} while (size);

out:
	release_sock(sk);
	return copied ? : err;
}
