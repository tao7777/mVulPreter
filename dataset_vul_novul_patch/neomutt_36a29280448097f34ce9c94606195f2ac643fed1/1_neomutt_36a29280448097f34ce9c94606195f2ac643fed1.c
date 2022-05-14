static int cmd_handle_untagged(struct ImapData *idata)
{
  unsigned int count = 0;
  char *s = imap_next_word(idata->buf);
  char *pn = imap_next_word(s);

  if ((idata->state >= IMAP_SELECTED) && isdigit((unsigned char) *s))
  {
    pn = s;
    s = imap_next_word(s);

    /* EXISTS and EXPUNGE are always related to the SELECTED mailbox for the
     * connection, so update that one.
     */
    if (mutt_str_strncasecmp("EXISTS", s, 6) == 0)
    {
      mutt_debug(2, "Handling EXISTS\n");

      /* new mail arrived */
      if (mutt_str_atoui(pn, &count) < 0)
      {
        mutt_debug(1, "Malformed EXISTS: '%s'\n", pn);
      }

      if (!(idata->reopen & IMAP_EXPUNGE_PENDING) && count < idata->max_msn)
      {
        /* Notes 6.0.3 has a tendency to report fewer messages exist than
         * it should. */
        mutt_debug(1, "Message count is out of sync\n");
        return 0;
      }
      /* at least the InterChange server sends EXISTS messages freely,
       * even when there is no new mail */
      else if (count == idata->max_msn)
        mutt_debug(3, "superfluous EXISTS message.\n");
      else
      {
        if (!(idata->reopen & IMAP_EXPUNGE_PENDING))
        {
          mutt_debug(2, "New mail in %s - %d messages total.\n", idata->mailbox, count);
          idata->reopen |= IMAP_NEWMAIL_PENDING;
        }
        idata->new_mail_count = count;
      }
    }
    /* pn vs. s: need initial seqno */
    else if (mutt_str_strncasecmp("EXPUNGE", s, 7) == 0)
      cmd_parse_expunge(idata, pn);
    else if (mutt_str_strncasecmp("FETCH", s, 5) == 0)
      cmd_parse_fetch(idata, pn);
  }
  else if (mutt_str_strncasecmp("CAPABILITY", s, 10) == 0)
    cmd_parse_capability(idata, s);
  else if (mutt_str_strncasecmp("OK [CAPABILITY", s, 14) == 0)
    cmd_parse_capability(idata, pn);
  else if (mutt_str_strncasecmp("OK [CAPABILITY", pn, 14) == 0)
    cmd_parse_capability(idata, imap_next_word(pn));
  else if (mutt_str_strncasecmp("LIST", s, 4) == 0)
    cmd_parse_list(idata, s);
  else if (mutt_str_strncasecmp("LSUB", s, 4) == 0)
    cmd_parse_lsub(idata, s);
  else if (mutt_str_strncasecmp("MYRIGHTS", s, 8) == 0)
    cmd_parse_myrights(idata, s);
  else if (mutt_str_strncasecmp("SEARCH", s, 6) == 0)
    cmd_parse_search(idata, s);
  else if (mutt_str_strncasecmp("STATUS", s, 6) == 0)
    cmd_parse_status(idata, s);
  else if (mutt_str_strncasecmp("ENABLED", s, 7) == 0)
    cmd_parse_enabled(idata, s);
  else if (mutt_str_strncasecmp("BYE", s, 3) == 0)
  {
    mutt_debug(2, "Handling BYE\n");

    /* check if we're logging out */
    if (idata->status == IMAP_BYE)
      return 0;

    /* server shut down our connection */
    s += 3;
    SKIPWS(s);
    mutt_error("%s", s);
    cmd_handle_fatal(idata);

    return -1;
  }
  else if (ImapServernoise && (mutt_str_strncasecmp("NO", s, 2) == 0))
  {
     mutt_debug(2, "Handling untagged NO\n");
 
     /* Display the warning message from the server */
    mutt_error("%s", s + 3);
   }
 
   return 0;
}
