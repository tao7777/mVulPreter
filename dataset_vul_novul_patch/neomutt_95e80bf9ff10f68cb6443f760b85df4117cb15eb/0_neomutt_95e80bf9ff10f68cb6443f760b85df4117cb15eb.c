int imap_subscribe(char *path, bool subscribe)
{
  struct ImapData *idata = NULL;
  char buf[LONG_STRING];
  char mbox[LONG_STRING];
   char errstr[STRING];
   struct Buffer err, token;
   struct ImapMbox mx;
  size_t len = 0;
 
   if (!mx_is_imap(path) || imap_parse_path(path, &mx) || !mx.mbox)
   {
    mutt_error(_("Bad mailbox name"));
    return -1;
  }
  idata = imap_conn_find(&(mx.account), 0);
  if (!idata)
    goto fail;

  imap_fix_path(idata, mx.mbox, buf, sizeof(buf));
  if (!*buf)
    mutt_str_strfcpy(buf, "INBOX", sizeof(buf));

  if (ImapCheckSubscribed)
  {
    mutt_buffer_init(&token);
     mutt_buffer_init(&err);
     err.data = errstr;
     err.dsize = sizeof(errstr);
	len = snprintf(mbox, sizeof(mbox), "%smailboxes ", subscribe ? "" : "un");
	imap_quote_string(mbox + len, sizeof(mbox) - len, path, true);
     if (mutt_parse_rc_line(mbox, &token, &err))
       mutt_debug(1, "Error adding subscribed mailbox: %s\n", errstr);
     FREE(&token.data);
  }

  if (subscribe)
    mutt_message(_("Subscribing to %s..."), buf);
  else
    mutt_message(_("Unsubscribing from %s..."), buf);
  imap_munge_mbox_name(idata, mbox, sizeof(mbox), buf);

  snprintf(buf, sizeof(buf), "%sSUBSCRIBE %s", subscribe ? "" : "UN", mbox);

  if (imap_exec(idata, buf, 0) < 0)
    goto fail;

  imap_unmunge_mbox_name(idata, mx.mbox);
  if (subscribe)
    mutt_message(_("Subscribed to %s"), mx.mbox);
  else
    mutt_message(_("Unsubscribed from %s"), mx.mbox);
  FREE(&mx.mbox);
  return 0;

fail:
  FREE(&mx.mbox);
  return -1;
}
