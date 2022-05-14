 mm_answer_pam_init_ctx(int sock, Buffer *m)
 {
 	debug3("%s", __func__);
	authctxt->user = buffer_get_string(m, NULL);
 	sshpam_ctxt = (sshpam_device.init_ctx)(authctxt);
 	sshpam_authok = NULL;
 	buffer_clear(m);
	if (sshpam_ctxt != NULL) {
		monitor_permit(mon_dispatch, MONITOR_REQ_PAM_FREE_CTX, 1);
		buffer_put_int(m, 1);
	} else {
		buffer_put_int(m, 0);
	}
	mm_request_send(sock, MONITOR_ANS_PAM_INIT_CTX, m);
	return (0);
}
