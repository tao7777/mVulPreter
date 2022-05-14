static int pop_fetch_headers(struct Context *ctx)
{
  struct PopData *pop_data = (struct PopData *) ctx->data;
  struct Progress progress;

#ifdef USE_HCACHE
  header_cache_t *hc = pop_hcache_open(pop_data, ctx->path);
#endif

  time(&pop_data->check_time);
  pop_data->clear_cache = false;

  for (int i = 0; i < ctx->msgcount; i++)
    ctx->hdrs[i]->refno = -1;

  const int old_count = ctx->msgcount;
  int ret = pop_fetch_data(pop_data, "UIDL\r\n", NULL, fetch_uidl, ctx);
  const int new_count = ctx->msgcount;
  ctx->msgcount = old_count;

  if (pop_data->cmd_uidl == 2)
  {
    if (ret == 0)
    {
      pop_data->cmd_uidl = 1;

      mutt_debug(1, "set UIDL capability\n");
    }

    if (ret == -2 && pop_data->cmd_uidl == 2)
    {
      pop_data->cmd_uidl = 0;

      mutt_debug(1, "unset UIDL capability\n");
      snprintf(pop_data->err_msg, sizeof(pop_data->err_msg), "%s",
               _("Command UIDL is not supported by server."));
    }
  }

  if (!ctx->quiet)
  {
    mutt_progress_init(&progress, _("Fetching message headers..."),
                       MUTT_PROGRESS_MSG, ReadInc, new_count - old_count);
  }

  if (ret == 0)
  {
    int i, deleted;
    for (i = 0, deleted = 0; i < old_count; i++)
    {
      if (ctx->hdrs[i]->refno == -1)
      {
        ctx->hdrs[i]->deleted = true;
        deleted++;
      }
    }
    if (deleted > 0)
    {
      mutt_error(
          ngettext("%d message has been lost. Try reopening the mailbox.",
                   "%d messages have been lost. Try reopening the mailbox.", deleted),
          deleted);
    }

    bool hcached = false;
    for (i = old_count; i < new_count; i++)
    {
      if (!ctx->quiet)
        mutt_progress_update(&progress, i + 1 - old_count, -1);
#ifdef USE_HCACHE
      void *data = mutt_hcache_fetch(hc, ctx->hdrs[i]->data, strlen(ctx->hdrs[i]->data));
      if (data)
      {
        char *uidl = mutt_str_strdup(ctx->hdrs[i]->data);
        int refno = ctx->hdrs[i]->refno;
        int index = ctx->hdrs[i]->index;
        /*
         * - POP dynamically numbers headers and relies on h->refno
         *   to map messages; so restore header and overwrite restored
         *   refno with current refno, same for index
         * - h->data needs to a separate pointer as it's driver-specific
         *   data freed separately elsewhere
         *   (the old h->data should point inside a malloc'd block from
         *   hcache so there shouldn't be a memleak here)
         */
        struct Header *h = mutt_hcache_restore((unsigned char *) data);
        mutt_hcache_free(hc, &data);
        mutt_header_free(&ctx->hdrs[i]);
        ctx->hdrs[i] = h;
        ctx->hdrs[i]->refno = refno;
        ctx->hdrs[i]->index = index;
        ctx->hdrs[i]->data = uidl;
        ret = 0;
        hcached = true;
      }
      else
#endif
          if ((ret = pop_read_header(pop_data, ctx->hdrs[i])) < 0)
        break;
#ifdef USE_HCACHE
      else
      {
        mutt_hcache_store(hc, ctx->hdrs[i]->data, strlen(ctx->hdrs[i]->data),
                          ctx->hdrs[i], 0);
      }
#endif

      /*
       * faked support for flags works like this:
       * - if 'hcached' is true, we have the message in our hcache:
       *        - if we also have a body: read
       *        - if we don't have a body: old
       *          (if $mark_old is set which is maybe wrong as
       *          $mark_old should be considered for syncing the
       *          folder and not when opening it XXX)
       * - if 'hcached' is false, we don't have the message in our hcache:
       *        - if we also have a body: read
        *        - if we don't have a body: new
        */
       const bool bcached =
          (mutt_bcache_exists(pop_data->bcache, ctx->hdrs[i]->data) == 0);
       ctx->hdrs[i]->old = false;
       ctx->hdrs[i]->read = false;
       if (hcached)
      {
        if (bcached)
          ctx->hdrs[i]->read = true;
        else if (MarkOld)
          ctx->hdrs[i]->old = true;
      }
      else
      {
        if (bcached)
          ctx->hdrs[i]->read = true;
      }

      ctx->msgcount++;
    }

    if (i > old_count)
      mx_update_context(ctx, i - old_count);
  }

#ifdef USE_HCACHE
  mutt_hcache_close(hc);
#endif

  if (ret < 0)
  {
    for (int i = ctx->msgcount; i < new_count; i++)
      mutt_header_free(&ctx->hdrs[i]);
    return ret;
  }

  /* after putting the result into our structures,
   * clean up cache, i.e. wipe messages deleted outside
   * the availability of our cache
   */
  if (MessageCacheClean)
    mutt_bcache_list(pop_data->bcache, msg_cache_check, (void *) ctx);

  mutt_clear_error();
  return (new_count - old_count);
}
