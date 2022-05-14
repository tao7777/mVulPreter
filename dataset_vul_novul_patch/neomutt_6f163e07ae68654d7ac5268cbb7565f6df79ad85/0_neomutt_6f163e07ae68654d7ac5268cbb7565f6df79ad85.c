enum ImapAuthRes imap_auth_cram_md5(struct ImapData *idata, const char *method)
{
  char ibuf[LONG_STRING * 2], obuf[LONG_STRING];
  unsigned char hmac_response[MD5_DIGEST_LEN];
  int len;
  int rc;

  if (!mutt_bit_isset(idata->capabilities, ACRAM_MD5))
    return IMAP_AUTH_UNAVAIL;

  mutt_message(_("Authenticating (CRAM-MD5)..."));

  /* get auth info */
  if (mutt_account_getlogin(&idata->conn->account) < 0)
    return IMAP_AUTH_FAILURE;
  if (mutt_account_getpass(&idata->conn->account) < 0)
    return IMAP_AUTH_FAILURE;

  imap_cmd_start(idata, "AUTHENTICATE CRAM-MD5");

  /* From RFC2195:
   * The data encoded in the first ready response contains a presumptively
   * arbitrary string of random digits, a timestamp, and the fully-qualified
   * primary host name of the server. The syntax of the unencoded form must
   * correspond to that of an RFC822 'msg-id' [RFC822] as described in [POP3].
   */
  do
    rc = imap_cmd_step(idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (rc != IMAP_CMD_RESPOND)
  {
    mutt_debug(1, "Invalid response from server: %s\n", ibuf);
     goto bail;
   }
 
  len = mutt_b64_decode(obuf, idata->buf + 2, sizeof(obuf));
   if (len == -1)
   {
     mutt_debug(1, "Error decoding base64 response.\n");
    goto bail;
  }

  obuf[len] = '\0';
  mutt_debug(2, "CRAM challenge: %s\n", obuf);

  /* The client makes note of the data and then responds with a string
   * consisting of the user name, a space, and a 'digest'. The latter is
   * computed by applying the keyed MD5 algorithm from [KEYED-MD5] where the
   * key is a shared secret and the digested text is the timestamp (including
   * angle-brackets).
   *
   * Note: The user name shouldn't be quoted. Since the digest can't contain
   *   spaces, there is no ambiguity. Some servers get this wrong, we'll work
   *   around them when the bug report comes in. Until then, we'll remain
   *   blissfully RFC-compliant.
   */
  hmac_md5(idata->conn->account.pass, obuf, hmac_response);
  /* dubious optimisation I saw elsewhere: make the whole string in one call */
  int off = snprintf(obuf, sizeof(obuf), "%s ", idata->conn->account.user);
  mutt_md5_toascii(hmac_response, obuf + off);
  mutt_debug(2, "CRAM response: %s\n", obuf);

  /* ibuf must be long enough to store the base64 encoding of obuf,
   * plus the additional debris */
  mutt_b64_encode(ibuf, obuf, strlen(obuf), sizeof(ibuf) - 2);
  mutt_str_strcat(ibuf, sizeof(ibuf), "\r\n");
  mutt_socket_send(idata->conn, ibuf);

  do
    rc = imap_cmd_step(idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (rc != IMAP_CMD_OK)
  {
    mutt_debug(1, "Error receiving server response.\n");
    goto bail;
  }

  if (imap_code(idata->buf))
    return IMAP_AUTH_SUCCESS;

bail:
  mutt_error(_("CRAM-MD5 authentication failed."));
  return IMAP_AUTH_FAILURE;
}
