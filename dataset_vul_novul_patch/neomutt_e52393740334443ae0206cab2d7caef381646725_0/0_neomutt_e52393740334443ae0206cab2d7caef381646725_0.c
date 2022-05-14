static void cmd_parse_lsub(struct ImapData *idata, char *s)
{
  char buf[STRING];
  char errstr[STRING];
  struct Buffer err, token;
  struct Url url;
  struct ImapList list;

  if (idata->cmddata && idata->cmdtype == IMAP_CT_LIST)
  {
    /* caller will handle response itself */
    cmd_parse_list(idata, s);
    return;
  }

  if (!ImapCheckSubscribed)
    return;

  idata->cmdtype = IMAP_CT_LIST;
  idata->cmddata = &list;
  cmd_parse_list(idata, s);
  idata->cmddata = NULL;
  /* noselect is for a gmail quirk (#3445) */
  if (!list.name || list.noselect)
    return;

  mutt_debug(3, "Subscribing to %s\n", list.name);

   mutt_str_strfcpy(buf, "mailboxes \"", sizeof(buf));
   mutt_account_tourl(&idata->conn->account, &url);
   /* escape \ and " */
  imap_quote_string(errstr, sizeof(errstr), list.name, true);
   url.path = errstr + 1;
   url.path[strlen(url.path) - 1] = '\0';
   if (mutt_str_strcmp(url.user, ImapUser) == 0)
    url.user = NULL;
  url_tostring(&url, buf + 11, sizeof(buf) - 11, 0);
  mutt_str_strcat(buf, sizeof(buf), "\"");
  mutt_buffer_init(&token);
  mutt_buffer_init(&err);
  err.data = errstr;
  err.dsize = sizeof(errstr);
  if (mutt_parse_rc_line(buf, &token, &err))
    mutt_debug(1, "Error adding subscribed mailbox: %s\n", errstr);
  FREE(&token.data);
}
