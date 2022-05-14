int sctp_do_peeloff(struct sock *sk, sctp_assoc_t id, struct socket **sockp)
{
	struct sctp_association *asoc = sctp_id2assoc(sk, id);
	struct sctp_sock *sp = sctp_sk(sk);
 	struct socket *sock;
 	int err = 0;
 
 	if (!asoc)
 		return -EINVAL;
 
	/* If there is a thread waiting on more sndbuf space for
	 * sending on this asoc, it cannot be peeled.
	 */
	if (waitqueue_active(&asoc->wait))
		return -EBUSY;

	/* An association cannot be branched off from an already peeled-off
	 * socket, nor is this supported for tcp style sockets.
	 */
	if (!sctp_style(sk, UDP))
		return -EINVAL;

	/* Create a new socket.  */
	err = sock_create(sk->sk_family, SOCK_SEQPACKET, IPPROTO_SCTP, &sock);
	if (err < 0)
		return err;

	sctp_copy_sock(sock->sk, sk, asoc);

	/* Make peeled-off sockets more like 1-1 accepted sockets.
	 * Set the daddr and initialize id to something more random
	 */
	sp->pf->to_sk_daddr(&asoc->peer.primary_addr, sk);

	/* Populate the fields of the newsk from the oldsk and migrate the
	 * asoc to the newsk.
	 */
	sctp_sock_migrate(sk, sock->sk, asoc, SCTP_SOCKET_UDP_HIGH_BANDWIDTH);

	*sockp = sock;

	return err;
}
