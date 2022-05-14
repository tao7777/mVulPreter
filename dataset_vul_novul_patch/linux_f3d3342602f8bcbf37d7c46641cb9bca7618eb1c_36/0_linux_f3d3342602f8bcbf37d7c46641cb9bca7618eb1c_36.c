static int unix_stream_recvmsg(struct kiocb *iocb, struct socket *sock,
			       struct msghdr *msg, size_t size,
			       int flags)
{
	struct sock_iocb *siocb = kiocb_to_siocb(iocb);
	struct scm_cookie tmp_scm;
	struct sock *sk = sock->sk;
	struct unix_sock *u = unix_sk(sk);
	struct sockaddr_un *sunaddr = msg->msg_name;
	int copied = 0;
	int check_creds = 0;
	int target;
	int err = 0;
	long timeo;
	int skip;

	err = -EINVAL;
	if (sk->sk_state != TCP_ESTABLISHED)
		goto out;

	err = -EOPNOTSUPP;
	if (flags&MSG_OOB)
		goto out;

 	target = sock_rcvlowat(sk, flags&MSG_WAITALL, size);
 	timeo = sock_rcvtimeo(sk, flags&MSG_DONTWAIT);
 
 	/* Lock the socket to prevent queue disordering
 	 * while sleeps in memcpy_tomsg
 	 */

	if (!siocb->scm) {
		siocb->scm = &tmp_scm;
		memset(&tmp_scm, 0, sizeof(tmp_scm));
	}

	err = mutex_lock_interruptible(&u->readlock);
	if (err) {
		err = sock_intr_errno(timeo);
		goto out;
	}

	do {
		int chunk;
		struct sk_buff *skb, *last;

		unix_state_lock(sk);
		last = skb = skb_peek(&sk->sk_receive_queue);
again:
		if (skb == NULL) {
			unix_sk(sk)->recursion_level = 0;
			if (copied >= target)
				goto unlock;

			/*
			 *	POSIX 1003.1g mandates this order.
			 */

			err = sock_error(sk);
			if (err)
				goto unlock;
			if (sk->sk_shutdown & RCV_SHUTDOWN)
				goto unlock;

			unix_state_unlock(sk);
			err = -EAGAIN;
			if (!timeo)
				break;
			mutex_unlock(&u->readlock);

			timeo = unix_stream_data_wait(sk, timeo, last);

			if (signal_pending(current)
			    ||  mutex_lock_interruptible(&u->readlock)) {
				err = sock_intr_errno(timeo);
				goto out;
			}

			continue;
 unlock:
			unix_state_unlock(sk);
			break;
		}

		skip = sk_peek_offset(sk, flags);
		while (skip >= unix_skb_len(skb)) {
			skip -= unix_skb_len(skb);
			last = skb;
			skb = skb_peek_next(skb, &sk->sk_receive_queue);
			if (!skb)
				goto again;
		}

		unix_state_unlock(sk);

		if (check_creds) {
			/* Never glue messages from different writers */
			if ((UNIXCB(skb).pid  != siocb->scm->pid) ||
			    !uid_eq(UNIXCB(skb).uid, siocb->scm->creds.uid) ||
			    !gid_eq(UNIXCB(skb).gid, siocb->scm->creds.gid))
				break;
		} else if (test_bit(SOCK_PASSCRED, &sock->flags)) {
			/* Copy credentials */
			scm_set_cred(siocb->scm, UNIXCB(skb).pid, UNIXCB(skb).uid, UNIXCB(skb).gid);
			check_creds = 1;
		}

		/* Copy address just once */
		if (sunaddr) {
			unix_copy_addr(msg, skb->sk);
			sunaddr = NULL;
		}

		chunk = min_t(unsigned int, unix_skb_len(skb) - skip, size);
		if (skb_copy_datagram_iovec(skb, UNIXCB(skb).consumed + skip,
					    msg->msg_iov, chunk)) {
			if (copied == 0)
				copied = -EFAULT;
			break;
		}
		copied += chunk;
		size -= chunk;

		/* Mark read part of skb as used */
		if (!(flags & MSG_PEEK)) {
			UNIXCB(skb).consumed += chunk;

			sk_peek_offset_bwd(sk, chunk);

			if (UNIXCB(skb).fp)
				unix_detach_fds(siocb->scm, skb);

			if (unix_skb_len(skb))
				break;

			skb_unlink(skb, &sk->sk_receive_queue);
			consume_skb(skb);

			if (siocb->scm->fp)
				break;
		} else {
			/* It is questionable, see note in unix_dgram_recvmsg.
			 */
			if (UNIXCB(skb).fp)
				siocb->scm->fp = scm_fp_dup(UNIXCB(skb).fp);

			sk_peek_offset_fwd(sk, chunk);

			break;
		}
	} while (size);

	mutex_unlock(&u->readlock);
	scm_recv(sock, msg, siocb->scm, flags);
out:
	return copied ? : err;
}
