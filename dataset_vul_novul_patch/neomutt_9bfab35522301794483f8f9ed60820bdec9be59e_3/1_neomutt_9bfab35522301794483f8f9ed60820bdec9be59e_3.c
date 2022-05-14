static int pop_sync_mailbox(struct Context *ctx, int *index_hint)
{
  int i, j, ret = 0;
  char buf[LONG_STRING];
  struct PopData *pop_data = (struct PopData *) ctx->data;
  struct Progress progress;
#ifdef USE_HCACHE
  header_cache_t *hc = NULL;
#endif

  pop_data->check_time = 0;

  while (true)
  {
    if (pop_reconnect(ctx) < 0)
      return -1;

    mutt_progress_init(&progress, _("Marking messages deleted..."),
                       MUTT_PROGRESS_MSG, WriteInc, ctx->deleted);

#ifdef USE_HCACHE
    hc = pop_hcache_open(pop_data, ctx->path);
#endif

    for (i = 0, j = 0, ret = 0; ret == 0 && i < ctx->msgcount; i++)
    {
      if (ctx->hdrs[i]->deleted && ctx->hdrs[i]->refno != -1)
      {
        j++;
        if (!ctx->quiet)
          mutt_progress_update(&progress, j, -1);
        snprintf(buf, sizeof(buf), "DELE %d\r\n", ctx->hdrs[i]->refno);
         ret = pop_query(pop_data, buf, sizeof(buf));
         if (ret == 0)
         {
          mutt_bcache_del(pop_data->bcache, ctx->hdrs[i]->data);
 #ifdef USE_HCACHE
           mutt_hcache_delete(hc, ctx->hdrs[i]->data, strlen(ctx->hdrs[i]->data));
 #endif
        }
      }

#ifdef USE_HCACHE
      if (ctx->hdrs[i]->changed)
      {
        mutt_hcache_store(hc, ctx->hdrs[i]->data, strlen(ctx->hdrs[i]->data),
                          ctx->hdrs[i], 0);
      }
#endif
    }

#ifdef USE_HCACHE
    mutt_hcache_close(hc);
#endif

    if (ret == 0)
    {
      mutt_str_strfcpy(buf, "QUIT\r\n", sizeof(buf));
      ret = pop_query(pop_data, buf, sizeof(buf));
    }

    if (ret == 0)
    {
      pop_data->clear_cache = true;
      pop_clear_cache(pop_data);
      pop_data->status = POP_DISCONNECTED;
      return 0;
    }

    if (ret == -2)
    {
      mutt_error("%s", pop_data->err_msg);
      return -1;
    }
  }
}
