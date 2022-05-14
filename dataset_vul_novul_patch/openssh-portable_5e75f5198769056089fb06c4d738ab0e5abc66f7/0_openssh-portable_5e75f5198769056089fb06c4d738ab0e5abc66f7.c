 mm_answer_pam_free_ctx(int sock, Buffer *m)
 {
	int r = sshpam_authok != NULL && sshpam_authok == sshpam_ctxt;
 
 	debug3("%s", __func__);
 	(sshpam_device.free_ctx)(sshpam_ctxt);
	sshpam_ctxt = sshpam_authok = NULL;
 	buffer_clear(m);
 	mm_request_send(sock, MONITOR_ANS_PAM_FREE_CTX, m);
 	auth_method = "keyboard-interactive";
 	auth_submethod = "pam";
	return r;
 }
