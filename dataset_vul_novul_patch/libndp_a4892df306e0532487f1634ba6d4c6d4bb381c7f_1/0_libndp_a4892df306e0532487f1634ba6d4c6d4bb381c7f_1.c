static int ndp_sock_recv(struct ndp *ndp)
{
	struct ndp_msg *msg;
	enum ndp_msg_type msg_type;
	size_t len;
	int err;

	msg = ndp_msg_alloc();
	if (!msg)
		return -ENOMEM;
 
 	len = ndp_msg_payload_maxlen(msg);
 	err = myrecvfrom6(ndp->sock, msg->buf, &len, 0,
			  &msg->addrto, &msg->ifindex, &msg->hoplimit);
 	if (err) {
 		err(ndp, "Failed to receive message");
 		goto free_msg;
 	}
	dbg(ndp, "rcvd from: %s, ifindex: %u, hoplimit: %d",
		 str_in6_addr(&msg->addrto), msg->ifindex, msg->hoplimit);

	if (msg->hoplimit != 255) {
		warn(ndp, "ignoring packet with bad hop limit (%d)", msg->hoplimit);
		err = 0;
		goto free_msg;
	}
 
 	if (len < sizeof(*msg->icmp6_hdr)) {
 		warn(ndp, "rcvd icmp6 packet too short (%luB)", len);
		err = 0;
		goto free_msg;
	}
	err = ndp_msg_type_by_raw_type(&msg_type, msg->icmp6_hdr->icmp6_type);
	if (err) {
		err = 0;
		goto free_msg;
	}
	ndp_msg_init(msg, msg_type);
	ndp_msg_payload_len_set(msg, len);

	if (!ndp_msg_check_valid(msg)) {
		warn(ndp, "rcvd invalid ND message");
		err = 0;
		goto free_msg;
	}

	dbg(ndp, "rcvd %s, len: %zuB",
		 ndp_msg_type_info(msg_type)->strabbr, len);

	if (!ndp_msg_check_opts(msg)) {
		err = 0;
		goto free_msg;
	}

	err = ndp_call_handlers(ndp, msg);;

free_msg:
	ndp_msg_destroy(msg);
	return err;
}
