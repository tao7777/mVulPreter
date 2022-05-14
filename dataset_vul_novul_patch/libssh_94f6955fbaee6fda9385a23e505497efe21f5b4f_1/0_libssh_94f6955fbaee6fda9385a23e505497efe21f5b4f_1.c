 SSH_PACKET_CALLBACK(ssh_packet_kexdh_init){
  int rc = SSH_ERROR;
   (void)type;
   (void)user;
 
  SSH_LOG(SSH_LOG_PACKET,"Received SSH_MSG_KEXDH_INIT");
  if(session->dh_handshake_state != DH_STATE_INIT){
    SSH_LOG(SSH_LOG_RARE,"Invalid state for SSH_MSG_KEXDH_INIT");
    goto error;
  }
  switch(session->next_crypto->kex_type){
      case SSH_KEX_DH_GROUP1_SHA1:
      case SSH_KEX_DH_GROUP14_SHA1:
        rc=ssh_server_kexdh_init(session, packet);
        break;
  #ifdef HAVE_ECDH
      case SSH_KEX_ECDH_SHA2_NISTP256:
        rc = ssh_server_ecdh_init(session, packet);
        break;
  #endif
  #ifdef HAVE_CURVE25519
      case SSH_KEX_CURVE25519_SHA256_LIBSSH_ORG:
    	rc = ssh_server_curve25519_init(session, packet);
    	break;
  #endif
      default:
         ssh_set_error(session,SSH_FATAL,"Wrong kex type in ssh_packet_kexdh_init");
         rc = SSH_ERROR;
   }

error:
  if (rc == SSH_ERROR) {
       session->session_state = SSH_SESSION_STATE_ERROR;
  }
 
   return SSH_PACKET_USED;
 }
