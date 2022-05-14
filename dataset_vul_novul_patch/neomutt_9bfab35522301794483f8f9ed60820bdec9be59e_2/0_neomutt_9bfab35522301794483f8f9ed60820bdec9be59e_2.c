static int pop_fetch_message(struct Context *ctx, struct Message *msg, int msgno)
{
  void *uidl = NULL;
  char buf[LONG_STRING];
  char path[PATH_MAX];
  struct Progress progressbar;
  struct PopData *pop_data = (struct PopData *) ctx->data;
  struct PopCache *cache = NULL;
  struct Header *h = ctx->hdrs[msgno];
   unsigned short bcache = 1;
 
   /* see if we already have the message in body cache */
  msg->fp = mutt_bcache_get(pop_data->bcache, cache_id(h->data));
   if (msg->fp)
     return 0;
 
  /*
   * see if we already have the message in our cache in
   * case $message_cachedir is unset
   */
  cache = &pop_data->cache[h->index % POP_CACHE_LEN];

  if (cache->path)
  {
    if (cache->index == h->index)
    {
      /* yes, so just return a pointer to the message */
      msg->fp = fopen(cache->path, "r");
      if (msg->fp)
        return 0;

      mutt_perror(cache->path);
      return -1;
    }
    else
    {
      /* clear the previous entry */
      unlink(cache->path);
      FREE(&cache->path);
    }
  }

  while (true)
  {
    if (pop_reconnect(ctx) < 0)
      return -1;

    /* verify that massage index is correct */
    if (h->refno < 0)
    {
      mutt_error(
          _("The message index is incorrect. Try reopening the mailbox."));
      return -1;
    }

    mutt_progress_init(&progressbar, _("Fetching message..."), MUTT_PROGRESS_SIZE,
                        NetInc, h->content->length + h->content->offset - 1);
 
     /* see if we can put in body cache; use our cache as fallback */
    msg->fp = mutt_bcache_put(pop_data->bcache, cache_id(h->data));
     if (!msg->fp)
     {
       /* no */
      bcache = 0;
      mutt_mktemp(path, sizeof(path));
      msg->fp = mutt_file_fopen(path, "w+");
      if (!msg->fp)
      {
        mutt_perror(path);
        return -1;
      }
    }

    snprintf(buf, sizeof(buf), "RETR %d\r\n", h->refno);

    const int ret = pop_fetch_data(pop_data, buf, &progressbar, fetch_message, msg->fp);
    if (ret == 0)
      break;

    mutt_file_fclose(&msg->fp);

    /* if RETR failed (e.g. connection closed), be sure to remove either
     * the file in bcache or from POP's own cache since the next iteration
     * of the loop will re-attempt to put() the message */
    if (!bcache)
      unlink(path);

    if (ret == -2)
    {
      mutt_error("%s", pop_data->err_msg);
      return -1;
    }

    if (ret == -3)
    {
      mutt_error(_("Can't write message to temporary file!"));
      return -1;
    }
  }

  /* Update the header information.  Previously, we only downloaded a
    * portion of the headers, those required for the main display.
    */
   if (bcache)
    mutt_bcache_commit(pop_data->bcache, cache_id(h->data));
   else
   {
     cache->index = h->index;
    cache->path = mutt_str_strdup(path);
  }
  rewind(msg->fp);
  uidl = h->data;

  /* we replace envelop, key in subj_hash has to be updated as well */
  if (ctx->subj_hash && h->env->real_subj)
    mutt_hash_delete(ctx->subj_hash, h->env->real_subj, h);
  mutt_label_hash_remove(ctx, h);
  mutt_env_free(&h->env);
  h->env = mutt_rfc822_read_header(msg->fp, h, 0, 0);
  if (ctx->subj_hash && h->env->real_subj)
    mutt_hash_insert(ctx->subj_hash, h->env->real_subj, h);
  mutt_label_hash_add(ctx, h);

  h->data = uidl;
  h->lines = 0;
  fgets(buf, sizeof(buf), msg->fp);
  while (!feof(msg->fp))
  {
    ctx->hdrs[msgno]->lines++;
    fgets(buf, sizeof(buf), msg->fp);
  }

  h->content->length = ftello(msg->fp) - h->content->offset;

  /* This needs to be done in case this is a multipart message */
  if (!WithCrypto)
    h->security = crypt_query(h->content);

  mutt_clear_error();
  rewind(msg->fp);

  return 0;
}
