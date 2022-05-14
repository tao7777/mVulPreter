SSH_PACKET_CALLBACK(ssh_packet_dh_reply){
  int rc;
   (void)type;
   (void)user;
   SSH_LOG(SSH_LOG_PROTOCOL,"Received SSH_KEXDH_REPLY");
  if(session->session_state!= SSH_SESSION_STATE_DH &&
 		session->dh_handshake_state != DH_STATE_INIT_SENT){
 	ssh_set_error(session,SSH_FATAL,"ssh_packet_dh_reply called in wrong state : %d:%d",
 			session->session_state,session->dh_handshake_state);
	goto error;
  }
  switch(session->next_crypto->kex_type){
    case SSH_KEX_DH_GROUP1_SHA1:
    case SSH_KEX_DH_GROUP14_SHA1:
      rc=ssh_client_dh_reply(session, packet);
      break;
#ifdef HAVE_ECDH
    case SSH_KEX_ECDH_SHA2_NISTP256:
      rc = ssh_client_ecdh_reply(session, packet);
      break;
#endif
#ifdef HAVE_CURVE25519
    case SSH_KEX_CURVE25519_SHA256_LIBSSH_ORG:
      rc = ssh_client_curve25519_reply(session, packet);
      break;
#endif
    default:
      ssh_set_error(session,SSH_FATAL,"Wrong kex type in ssh_packet_dh_reply");
      goto error;
  }
  if(rc==SSH_OK) {
    session->dh_handshake_state = DH_STATE_NEWKEYS_SENT;
    return SSH_PACKET_USED;
  }
error:
  session->session_state=SSH_SESSION_STATE_ERROR;
  return SSH_PACKET_USED;
}
