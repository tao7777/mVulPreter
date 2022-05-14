 ssh_packet_set_postauth(struct ssh *ssh)
 {
	struct sshcomp *comp;
	int r, mode;
 
 	debug("%s: called", __func__);
 	/* This was set in net child, but is not visible in user child */
 	ssh->state->after_authentication = 1;
 	ssh->state->rekeying = 0;
	for (mode = 0; mode < MODE_MAX; mode++) {
		if (ssh->state->newkeys[mode] == NULL)
			continue;
		comp = &ssh->state->newkeys[mode]->comp;
		if (comp && comp->enabled &&
		    (r = ssh_packet_init_compression(ssh)) != 0)
			return r;
	}
 	return 0;
 }
