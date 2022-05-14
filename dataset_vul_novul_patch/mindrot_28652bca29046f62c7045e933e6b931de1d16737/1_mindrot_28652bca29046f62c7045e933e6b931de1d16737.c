kex_input_newkeys(int type, u_int32_t seq, void *ctxt)
{
	struct ssh *ssh = ctxt;
	struct kex *kex = ssh->kex;
	int r;

	debug("SSH2_MSG_NEWKEYS received");
 	ssh_dispatch_set(ssh, SSH2_MSG_NEWKEYS, &kex_protocol_error);
 	if ((r = sshpkt_get_end(ssh)) != 0)
 		return r;
 	kex->done = 1;
 	sshbuf_reset(kex->peer);
 	/* sshbuf_reset(kex->my); */
	kex->name = NULL;
	return 0;
}
