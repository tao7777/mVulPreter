server_request_direct_streamlocal(void)
{
	Channel *c = NULL;
	char *target, *originator;
	u_short originator_port;

	target = packet_get_string(NULL);
	originator = packet_get_string(NULL);
	originator_port = packet_get_int();
	packet_check_eom();

	debug("server_request_direct_streamlocal: originator %s port %d, target %s",
	    originator, originator_port, target);
 
 	/* XXX fine grained permissions */
 	if ((options.allow_streamlocal_forwarding & FORWARD_LOCAL) != 0 &&
	    !no_port_forwarding_flag && !options.disable_forwarding &&
	    use_privsep) {
 		c = channel_connect_to_path(target,
 		    "direct-streamlocal@openssh.com", "direct-streamlocal");
 	} else {
		logit("refused streamlocal port forward: "
		    "originator %s port %d, target %s",
		    originator, originator_port, target);
	}

	free(originator);
	free(target);

	return c;
}
