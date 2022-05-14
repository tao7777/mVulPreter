 ssh_packet_set_postauth(struct ssh *ssh)
 {
	int r;
 
 	debug("%s: called", __func__);
 	/* This was set in net child, but is not visible in user child */
 	ssh->state->after_authentication = 1;
 	ssh->state->rekeying = 0;
	if ((r = ssh_packet_enable_delayed_compress(ssh)) != 0)
		return r;
 	return 0;
 }
