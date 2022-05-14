enum ImapAuthRes imap_auth_gss(struct ImapData *idata, const char *method)
{
  gss_buffer_desc request_buf, send_token;
  gss_buffer_t sec_token;
  gss_name_t target_name;
  gss_ctx_id_t context;
  gss_OID mech_name;
  char server_conf_flags;
  gss_qop_t quality;
  int cflags;
  OM_uint32 maj_stat, min_stat;
  char buf1[GSS_BUFSIZE], buf2[GSS_BUFSIZE];
  unsigned long buf_size;
  int rc;

  if (!mutt_bit_isset(idata->capabilities, AGSSAPI))
    return IMAP_AUTH_UNAVAIL;

  if (mutt_account_getuser(&idata->conn->account) < 0)
    return IMAP_AUTH_FAILURE;

  /* get an IMAP service ticket for the server */
  snprintf(buf1, sizeof(buf1), "imap@%s", idata->conn->account.host);
  request_buf.value = buf1;
  request_buf.length = strlen(buf1);
  maj_stat = gss_import_name(&min_stat, &request_buf, gss_nt_service_name, &target_name);
  if (maj_stat != GSS_S_COMPLETE)
  {
    mutt_debug(2, "Couldn't get service name for [%s]\n", buf1);
    return IMAP_AUTH_UNAVAIL;
  }
  else if (DebugLevel >= 2)
  {
    gss_display_name(&min_stat, target_name, &request_buf, &mech_name);
    mutt_debug(2, "Using service name [%s]\n", (char *) request_buf.value);
    gss_release_buffer(&min_stat, &request_buf);
  }
  /* Acquire initial credentials - without a TGT GSSAPI is UNAVAIL */
  sec_token = GSS_C_NO_BUFFER;
  context = GSS_C_NO_CONTEXT;

  /* build token */
  maj_stat = gss_init_sec_context(&min_stat, GSS_C_NO_CREDENTIAL, &context, target_name,
                                  GSS_C_NO_OID, GSS_C_MUTUAL_FLAG | GSS_C_SEQUENCE_FLAG,
                                  0, GSS_C_NO_CHANNEL_BINDINGS, sec_token, NULL,
                                  &send_token, (unsigned int *) &cflags, NULL);
  if (maj_stat != GSS_S_COMPLETE && maj_stat != GSS_S_CONTINUE_NEEDED)
  {
    print_gss_error(maj_stat, min_stat);
    mutt_debug(1, "Error acquiring credentials - no TGT?\n");
    gss_release_name(&min_stat, &target_name);

    return IMAP_AUTH_UNAVAIL;
  }

  /* now begin login */
  mutt_message(_("Authenticating (GSSAPI)..."));

  imap_cmd_start(idata, "AUTHENTICATE GSSAPI");

  /* expect a null continuation response ("+") */
  do
    rc = imap_cmd_step(idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (rc != IMAP_CMD_RESPOND)
  {
    mutt_debug(2, "Invalid response from server: %s\n", buf1);
    gss_release_name(&min_stat, &target_name);
    goto bail;
  }

  /* now start the security context initialisation loop... */
  mutt_debug(2, "Sending credentials\n");
  mutt_b64_encode(buf1, send_token.value, send_token.length, sizeof(buf1) - 2);
  gss_release_buffer(&min_stat, &send_token);
  mutt_str_strcat(buf1, sizeof(buf1), "\r\n");
  mutt_socket_send(idata->conn, buf1);

  while (maj_stat == GSS_S_CONTINUE_NEEDED)
  {
    /* Read server data */
    do
      rc = imap_cmd_step(idata);
    while (rc == IMAP_CMD_CONTINUE);

    if (rc != IMAP_CMD_RESPOND)
    {
      mutt_debug(1, "#1 Error receiving server response.\n");
      gss_release_name(&min_stat, &target_name);
       goto bail;
     }
 
    request_buf.length = mutt_b64_decode(buf2, idata->buf + 2, sizeof(buf2));
     request_buf.value = buf2;
     sec_token = &request_buf;
 
    /* Write client data */
    maj_stat = gss_init_sec_context(
        &min_stat, GSS_C_NO_CREDENTIAL, &context, target_name, GSS_C_NO_OID,
        GSS_C_MUTUAL_FLAG | GSS_C_SEQUENCE_FLAG, 0, GSS_C_NO_CHANNEL_BINDINGS,
        sec_token, NULL, &send_token, (unsigned int *) &cflags, NULL);
    if (maj_stat != GSS_S_COMPLETE && maj_stat != GSS_S_CONTINUE_NEEDED)
    {
      print_gss_error(maj_stat, min_stat);
      mutt_debug(1, "Error exchanging credentials\n");
      gss_release_name(&min_stat, &target_name);

      goto err_abort_cmd;
    }
    mutt_b64_encode(buf1, send_token.value, send_token.length, sizeof(buf1) - 2);
    gss_release_buffer(&min_stat, &send_token);
    mutt_str_strcat(buf1, sizeof(buf1), "\r\n");
    mutt_socket_send(idata->conn, buf1);
  }

  gss_release_name(&min_stat, &target_name);

  /* get security flags and buffer size */
  do
    rc = imap_cmd_step(idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (rc != IMAP_CMD_RESPOND)
  {
     mutt_debug(1, "#2 Error receiving server response.\n");
     goto bail;
   }
  request_buf.length = mutt_b64_decode(buf2, idata->buf + 2, sizeof(buf2));
   request_buf.value = buf2;
 
   maj_stat = gss_unwrap(&min_stat, context, &request_buf, &send_token, &cflags, &quality);
  if (maj_stat != GSS_S_COMPLETE)
  {
    print_gss_error(maj_stat, min_stat);
    mutt_debug(2, "Couldn't unwrap security level data\n");
    gss_release_buffer(&min_stat, &send_token);
    goto err_abort_cmd;
  }
  mutt_debug(2, "Credential exchange complete\n");

  /* first octet is security levels supported. We want NONE */
  server_conf_flags = ((char *) send_token.value)[0];
  if (!(((char *) send_token.value)[0] & GSS_AUTH_P_NONE))
  {
    mutt_debug(2, "Server requires integrity or privacy\n");
    gss_release_buffer(&min_stat, &send_token);
    goto err_abort_cmd;
  }

  /* we don't care about buffer size if we don't wrap content. But here it is */
  ((char *) send_token.value)[0] = '\0';
  buf_size = ntohl(*((long *) send_token.value));
  gss_release_buffer(&min_stat, &send_token);
  mutt_debug(2, "Unwrapped security level flags: %c%c%c\n",
             (server_conf_flags & GSS_AUTH_P_NONE) ? 'N' : '-',
             (server_conf_flags & GSS_AUTH_P_INTEGRITY) ? 'I' : '-',
             (server_conf_flags & GSS_AUTH_P_PRIVACY) ? 'P' : '-');
  mutt_debug(2, "Maximum GSS token size is %ld\n", buf_size);

  /* agree to terms (hack!) */
  buf_size = htonl(buf_size); /* not relevant without integrity/privacy */
  memcpy(buf1, &buf_size, 4);
  buf1[0] = GSS_AUTH_P_NONE;
  /* server decides if principal can log in as user */
  strncpy(buf1 + 4, idata->conn->account.user, sizeof(buf1) - 4);
  request_buf.value = buf1;
  request_buf.length = 4 + strlen(idata->conn->account.user);
  maj_stat = gss_wrap(&min_stat, context, 0, GSS_C_QOP_DEFAULT, &request_buf,
                      &cflags, &send_token);
  if (maj_stat != GSS_S_COMPLETE)
  {
    mutt_debug(2, "Error creating login request\n");
    goto err_abort_cmd;
  }

  mutt_b64_encode(buf1, send_token.value, send_token.length, sizeof(buf1) - 2);
  mutt_debug(2, "Requesting authorisation as %s\n", idata->conn->account.user);
  mutt_str_strcat(buf1, sizeof(buf1), "\r\n");
  mutt_socket_send(idata->conn, buf1);

  /* Joy of victory or agony of defeat? */
  do
    rc = imap_cmd_step(idata);
  while (rc == IMAP_CMD_CONTINUE);
  if (rc == IMAP_CMD_RESPOND)
  {
    mutt_debug(1, "Unexpected server continuation request.\n");
    goto err_abort_cmd;
  }
  if (imap_code(idata->buf))
  {
    /* flush the security context */
    mutt_debug(2, "Releasing GSS credentials\n");
    maj_stat = gss_delete_sec_context(&min_stat, &context, &send_token);
    if (maj_stat != GSS_S_COMPLETE)
      mutt_debug(1, "Error releasing credentials\n");

    /* send_token may contain a notification to the server to flush
     * credentials. RFC1731 doesn't specify what to do, and since this
     * support is only for authentication, we'll assume the server knows
     * enough to flush its own credentials */
    gss_release_buffer(&min_stat, &send_token);

    return IMAP_AUTH_SUCCESS;
  }
  else
    goto bail;

err_abort_cmd:
  mutt_socket_send(idata->conn, "*\r\n");
  do
    rc = imap_cmd_step(idata);
  while (rc == IMAP_CMD_CONTINUE);

bail:
  mutt_error(_("GSSAPI authentication failed."));
  return IMAP_AUTH_FAILURE;
}
