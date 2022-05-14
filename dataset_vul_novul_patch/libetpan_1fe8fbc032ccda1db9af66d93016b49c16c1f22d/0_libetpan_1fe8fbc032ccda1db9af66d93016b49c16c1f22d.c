static int mailimf_group_parse(const char * message, size_t length,
			       size_t * indx,
			       struct mailimf_group ** result)
{
  size_t cur_token;
  char * display_name;
  struct mailimf_mailbox_list * mailbox_list;
   struct mailimf_group * group;
   int r;
   int res;
  clist * list;
 
   cur_token = * indx;
 
  mailbox_list = NULL;

  r = mailimf_display_name_parse(message, length, &cur_token, &display_name);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_display_name;
  }

  r = mailimf_mailbox_list_parse(message, length, &cur_token, &mailbox_list);
  switch (r) {
  case MAILIMF_NO_ERROR:
    break;
  case MAILIMF_ERROR_PARSE:
    r = mailimf_cfws_parse(message, length, &cur_token);
    if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
       res = r;
       goto free_display_name;
     }
    list = clist_new();
    if (list == NULL) {
      res = MAILIMF_ERROR_MEMORY;
      goto free_display_name;
    }
    mailbox_list = mailimf_mailbox_list_new(list);
    if (mailbox_list == NULL) {
      res = MAILIMF_ERROR_MEMORY;
      clist_free(list);
      goto free_display_name;
    }
     break;
   default:
     res = r;
    goto free_display_name;
  }

  r = mailimf_semi_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_mailbox_list;
  }

  group = mailimf_group_new(display_name, mailbox_list);
  if (group == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_mailbox_list;
  }

  * indx = cur_token;
  * result = group;

  return MAILIMF_NO_ERROR;

 free_mailbox_list:
  if (mailbox_list != NULL) {
    mailimf_mailbox_list_free(mailbox_list);
  }
 free_display_name:
  mailimf_display_name_free(display_name);
 err:
  return res;
}
