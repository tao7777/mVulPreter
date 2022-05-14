enum ImapAuthRes imap_auth_login(struct ImapData *idata, const char *method)
{
  char q_user[SHORT_STRING], q_pass[SHORT_STRING];
  char buf[STRING];
  int rc;

  if (mutt_bit_isset(idata->capabilities, LOGINDISABLED))
  {
    mutt_message(_("LOGIN disabled on this server."));
    return IMAP_AUTH_UNAVAIL;
  }

  if (mutt_account_getuser(&idata->conn->account) < 0)
    return IMAP_AUTH_FAILURE;
  if (mutt_account_getpass(&idata->conn->account) < 0)
    return IMAP_AUTH_FAILURE;
 
   mutt_message(_("Logging in..."));
 
  imap_quote_string(q_user, sizeof(q_user), idata->conn->account.user);
  imap_quote_string(q_pass, sizeof(q_pass), idata->conn->account.pass);
 
   /* don't print the password unless we're at the ungodly debugging level
    * of 5 or higher */

  if (DebugLevel < IMAP_LOG_PASS)
    mutt_debug(2, "Sending LOGIN command for %s...\n", idata->conn->account.user);

  snprintf(buf, sizeof(buf), "LOGIN %s %s", q_user, q_pass);
  rc = imap_exec(idata, buf, IMAP_CMD_FAIL_OK | IMAP_CMD_PASS);

  if (!rc)
  {
    mutt_clear_error(); /* clear "Logging in...".  fixes #3524 */
    return IMAP_AUTH_SUCCESS;
  }

  mutt_error(_("Login failed."));
  return IMAP_AUTH_FAILURE;
}
