int x25_negotiate_facilities(struct sk_buff *skb, struct sock *sk,
		struct x25_facilities *new, struct x25_dte_facilities *dte)
{
	struct x25_sock *x25 = x25_sk(sk);
	struct x25_facilities *ours = &x25->facilities;
	struct x25_facilities theirs;
	int len;
 
 	memset(&theirs, 0, sizeof(theirs));
 	memcpy(new, ours, sizeof(*new));
	memset(dte, 0, sizeof(*dte));
 
 	len = x25_parse_facilities(skb, &theirs, dte, &x25->vc_facil_mask);
 	if (len < 0)
		return len;

	/*
	 *	They want reverse charging, we won't accept it.
	 */
	if ((theirs.reverse & 0x01 ) && (ours->reverse & 0x01)) {
		SOCK_DEBUG(sk, "X.25: rejecting reverse charging request\n");
		return -1;
	}

	new->reverse = theirs.reverse;

	if (theirs.throughput) {
		int theirs_in =  theirs.throughput & 0x0f;
		int theirs_out = theirs.throughput & 0xf0;
		int ours_in  = ours->throughput & 0x0f;
		int ours_out = ours->throughput & 0xf0;
		if (!ours_in || theirs_in < ours_in) {
			SOCK_DEBUG(sk, "X.25: inbound throughput negotiated\n");
			new->throughput = (new->throughput & 0xf0) | theirs_in;
		}
		if (!ours_out || theirs_out < ours_out) {
			SOCK_DEBUG(sk,
				"X.25: outbound throughput negotiated\n");
			new->throughput = (new->throughput & 0x0f) | theirs_out;
		}
	}

	if (theirs.pacsize_in && theirs.pacsize_out) {
		if (theirs.pacsize_in < ours->pacsize_in) {
			SOCK_DEBUG(sk, "X.25: packet size inwards negotiated down\n");
			new->pacsize_in = theirs.pacsize_in;
		}
		if (theirs.pacsize_out < ours->pacsize_out) {
			SOCK_DEBUG(sk, "X.25: packet size outwards negotiated down\n");
			new->pacsize_out = theirs.pacsize_out;
		}
	}

	if (theirs.winsize_in && theirs.winsize_out) {
		if (theirs.winsize_in < ours->winsize_in) {
			SOCK_DEBUG(sk, "X.25: window size inwards negotiated down\n");
			new->winsize_in = theirs.winsize_in;
		}
		if (theirs.winsize_out < ours->winsize_out) {
			SOCK_DEBUG(sk, "X.25: window size outwards negotiated down\n");
			new->winsize_out = theirs.winsize_out;
		}
	}

	return len;
}
