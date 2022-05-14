static int caif_stream_recvmsg(struct kiocb *iocb, struct socket *sock,
			       struct msghdr *msg, size_t size,
			       int flags)
{
	struct sock *sk = sock->sk;
	int copied = 0;
	int target;
	int err = 0;
	long timeo;

	err = -EOPNOTSUPP;
 	if (flags&MSG_OOB)
 		goto out;
 
 	/*
 	 * Lock the socket to prevent queue disordering
 	 * while sleeps in memcpy_tomsg
	 */
	err = -EAGAIN;
	if (sk->sk_state == CAIF_CONNECTING)
		goto out;

	caif_read_lock(sk);
	target = sock_rcvlowat(sk, flags&MSG_WAITALL, size);
	timeo = sock_rcvtimeo(sk, flags&MSG_DONTWAIT);

	do {
		int chunk;
		struct sk_buff *skb;

		lock_sock(sk);
		skb = skb_dequeue(&sk->sk_receive_queue);
		caif_check_flow_release(sk);

		if (skb == NULL) {
			if (copied >= target)
				goto unlock;
			/*
			 *	POSIX 1003.1g mandates this order.
			 */
			err = sock_error(sk);
			if (err)
				goto unlock;
			err = -ECONNRESET;
			if (sk->sk_shutdown & RCV_SHUTDOWN)
				goto unlock;

			err = -EPIPE;
			if (sk->sk_state != CAIF_CONNECTED)
				goto unlock;
			if (sock_flag(sk, SOCK_DEAD))
				goto unlock;

			release_sock(sk);

			err = -EAGAIN;
			if (!timeo)
				break;

			caif_read_unlock(sk);

			timeo = caif_stream_data_wait(sk, timeo);

			if (signal_pending(current)) {
				err = sock_intr_errno(timeo);
				goto out;
			}
			caif_read_lock(sk);
			continue;
unlock:
			release_sock(sk);
			break;
		}
		release_sock(sk);
		chunk = min_t(unsigned int, skb->len, size);
		if (memcpy_toiovec(msg->msg_iov, skb->data, chunk)) {
			skb_queue_head(&sk->sk_receive_queue, skb);
			if (copied == 0)
				copied = -EFAULT;
			break;
		}
		copied += chunk;
		size -= chunk;

		/* Mark read part of skb as used */
		if (!(flags & MSG_PEEK)) {
			skb_pull(skb, chunk);

			/* put the skb back if we didn't use it up. */
			if (skb->len) {
				skb_queue_head(&sk->sk_receive_queue, skb);
				break;
			}
			kfree_skb(skb);

		} else {
			/*
			 * It is questionable, see note in unix_dgram_recvmsg.
			 */
			/* put message back and return */
			skb_queue_head(&sk->sk_receive_queue, skb);
			break;
		}
	} while (size);
	caif_read_unlock(sk);

out:
	return copied ? : err;
}
