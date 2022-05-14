static void reply_sesssetup_and_X_spnego(struct smb_request *req)
{
	const uint8 *p;
	DATA_BLOB blob1;
	size_t bufrem;
	char *tmp;
	const char *native_os;
	const char *native_lanman;
	const char *primary_domain;
	const char *p2;
	uint16 data_blob_len = SVAL(req->vwv+7, 0);
	enum remote_arch_types ra_type = get_remote_arch();
	int vuid = req->vuid;
	user_struct *vuser = NULL;
	NTSTATUS status = NT_STATUS_OK;
	uint16 smbpid = req->smbpid;
	struct smbd_server_connection *sconn = smbd_server_conn;

	DEBUG(3,("Doing spnego session setup\n"));

	if (global_client_caps == 0) {
		global_client_caps = IVAL(req->vwv+10, 0);

		if (!(global_client_caps & CAP_STATUS32)) {
			remove_from_common_flags2(FLAGS2_32_BIT_ERROR_CODES);
		}

	}

	p = req->buf;

	if (data_blob_len == 0) {
		/* an invalid request */
		reply_nterror(req, nt_status_squash(NT_STATUS_LOGON_FAILURE));
		return;
	}

	bufrem = smbreq_bufrem(req, p);
	/* pull the spnego blob */
	blob1 = data_blob(p, MIN(bufrem, data_blob_len));

#if 0
        file_save("negotiate.dat", blob1.data, blob1.length);
 #endif
 
       p2 = (char *)req->buf + blob1.length;
 
        p2 += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p2,
                                     STR_TERMINATE);
	native_os = tmp ? tmp : "";

	p2 += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p2,
				     STR_TERMINATE);
	native_lanman = tmp ? tmp : "";

	p2 += srvstr_pull_req_talloc(talloc_tos(), req, &tmp, p2,
				     STR_TERMINATE);
	primary_domain = tmp ? tmp : "";

	DEBUG(3,("NativeOS=[%s] NativeLanMan=[%s] PrimaryDomain=[%s]\n",
		native_os, native_lanman, primary_domain));

	if ( ra_type == RA_WIN2K ) {
		/* Vista sets neither the OS or lanman strings */

		if ( !strlen(native_os) && !strlen(native_lanman) )
			set_remote_arch(RA_VISTA);

		/* Windows 2003 doesn't set the native lanman string,
		   but does set primary domain which is a bug I think */

		if ( !strlen(native_lanman) ) {
			ra_lanman_string( primary_domain );
		} else {
			ra_lanman_string( native_lanman );
		}
	}

	/* Did we get a valid vuid ? */
	if (!is_partial_auth_vuid(sconn, vuid)) {
		/* No, then try and see if this is an intermediate sessionsetup
		 * for a large SPNEGO packet. */
		struct pending_auth_data *pad;
		pad = get_pending_auth_data(sconn, smbpid);
		if (pad) {
			DEBUG(10,("reply_sesssetup_and_X_spnego: found "
				"pending vuid %u\n",
				(unsigned int)pad->vuid ));
			vuid = pad->vuid;
		}
	}

	/* Do we have a valid vuid now ? */
	if (!is_partial_auth_vuid(sconn, vuid)) {
		/* No, start a new authentication setup. */
		vuid = register_initial_vuid(sconn);
		if (vuid == UID_FIELD_INVALID) {
			data_blob_free(&blob1);
			reply_nterror(req, nt_status_squash(
					      NT_STATUS_INVALID_PARAMETER));
			return;
		}
	}

	vuser = get_partial_auth_user_struct(sconn, vuid);
	/* This MUST be valid. */
	if (!vuser) {
		smb_panic("reply_sesssetup_and_X_spnego: invalid vuid.");
	}

	/* Large (greater than 4k) SPNEGO blobs are split into multiple
	 * sessionsetup requests as the Windows limit on the security blob
	 * field is 4k. Bug #4400. JRA.
	 */

	status = check_spnego_blob_complete(sconn, smbpid, vuid, &blob1);
	if (!NT_STATUS_IS_OK(status)) {
		if (!NT_STATUS_EQUAL(status,
				NT_STATUS_MORE_PROCESSING_REQUIRED)) {
			/* Real error - kill the intermediate vuid */
			invalidate_vuid(sconn, vuid);
		}
		data_blob_free(&blob1);
		reply_nterror(req, nt_status_squash(status));
		return;
	}

	if (blob1.data[0] == ASN1_APPLICATION(0)) {

		/* its a negTokenTarg packet */

		reply_spnego_negotiate(req, vuid, blob1,
				       &vuser->auth_ntlmssp_state);
		data_blob_free(&blob1);
		return;
	}

	if (blob1.data[0] == ASN1_CONTEXT(1)) {

		/* its a auth packet */

		reply_spnego_auth(req, vuid, blob1,
				  &vuser->auth_ntlmssp_state);
		data_blob_free(&blob1);
		return;
	}

	if (strncmp((char *)(blob1.data), "NTLMSSP", 7) == 0) {
		DATA_BLOB chal;

		if (!vuser->auth_ntlmssp_state) {
			status = auth_ntlmssp_start(&vuser->auth_ntlmssp_state);
			if (!NT_STATUS_IS_OK(status)) {
				/* Kill the intermediate vuid */
				invalidate_vuid(sconn, vuid);
				data_blob_free(&blob1);
				reply_nterror(req, nt_status_squash(status));
				return;
			}
		}

		status = auth_ntlmssp_update(vuser->auth_ntlmssp_state,
						blob1, &chal);

		data_blob_free(&blob1);

		reply_spnego_ntlmssp(req, vuid,
				     &vuser->auth_ntlmssp_state,
				     &chal, status, OID_NTLMSSP, false);
		data_blob_free(&chal);
		return;
	}

	/* what sort of packet is this? */
	DEBUG(1,("Unknown packet in reply_sesssetup_and_X_spnego\n"));

	data_blob_free(&blob1);

	reply_nterror(req, nt_status_squash(NT_STATUS_LOGON_FAILURE));
}
