static int irda_recvmsg_stream(struct kiocb *iocb, struct socket *sock,
			       struct msghdr *msg, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self = irda_sk(sk);
	int noblock = flags & MSG_DONTWAIT;
	size_t copied = 0;
	int target, err;
	long timeo;

	IRDA_DEBUG(3, "%s()\n", __func__);

	if ((err = sock_error(sk)) < 0)
		return err;

	if (sock->flags & __SO_ACCEPTCON)
		return -EINVAL;

	err =-EOPNOTSUPP;
	if (flags & MSG_OOB)
		return -EOPNOTSUPP;

	err = 0;
 	target = sock_rcvlowat(sk, flags & MSG_WAITALL, size);
 	timeo = sock_rcvtimeo(sk, noblock);
 
 	do {
 		int chunk;
 		struct sk_buff *skb = skb_dequeue(&sk->sk_receive_queue);

		if (skb == NULL) {
			DEFINE_WAIT(wait);
			err = 0;

			if (copied >= target)
				break;

			prepare_to_wait_exclusive(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);

			/*
			 *	POSIX 1003.1g mandates this order.
			 */
			err = sock_error(sk);
			if (err)
				;
			else if (sk->sk_shutdown & RCV_SHUTDOWN)
				;
			else if (noblock)
				err = -EAGAIN;
			else if (signal_pending(current))
				err = sock_intr_errno(timeo);
			else if (sk->sk_state != TCP_ESTABLISHED)
				err = -ENOTCONN;
			else if (skb_peek(&sk->sk_receive_queue) == NULL)
				/* Wait process until data arrives */
				schedule();

			finish_wait(sk_sleep(sk), &wait);

			if (err)
				return err;
			if (sk->sk_shutdown & RCV_SHUTDOWN)
				break;

			continue;
		}

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

			/* put the skb back if we didn't use it up.. */
			if (skb->len) {
				IRDA_DEBUG(1, "%s(), back on q!\n",
					   __func__);
				skb_queue_head(&sk->sk_receive_queue, skb);
				break;
			}

			kfree_skb(skb);
		} else {
			IRDA_DEBUG(0, "%s() questionable!?\n", __func__);

			/* put message back and return */
			skb_queue_head(&sk->sk_receive_queue, skb);
			break;
		}
	} while (size);

	/*
	 *  Check if we have previously stopped IrTTP and we know
	 *  have more free space in our rx_queue. If so tell IrTTP
	 *  to start delivering frames again before our rx_queue gets
	 *  empty
	 */
	if (self->rx_flow == FLOW_STOP) {
		if ((atomic_read(&sk->sk_rmem_alloc) << 2) <= sk->sk_rcvbuf) {
			IRDA_DEBUG(2, "%s(), Starting IrTTP\n", __func__);
			self->rx_flow = FLOW_START;
			irttp_flow_request(self->tsap, FLOW_START);
		}
	}

	return copied;
}
