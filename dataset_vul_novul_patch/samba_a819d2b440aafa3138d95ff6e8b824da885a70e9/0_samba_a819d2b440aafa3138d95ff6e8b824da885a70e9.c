uint8_t smb2cli_session_security_mode(struct smbXcli_session *session)
{
	struct smbXcli_conn *conn = session->conn;
	uint8_t security_mode = 0;

	if (conn == NULL) {
		return security_mode;
	}

	security_mode = SMB2_NEGOTIATE_SIGNING_ENABLED;
        if (conn->mandatory_signing) {
                security_mode |= SMB2_NEGOTIATE_SIGNING_REQUIRED;
        }
       if (session->smb2->should_sign) {
               security_mode |= SMB2_NEGOTIATE_SIGNING_REQUIRED;
       }
 
        return security_mode;
 }
