static int nntp_fetch_headers(struct Context *ctx, void *hc, anum_t first,
                              anum_t last, int restore)
{
  struct NntpData *nntp_data = ctx->data;
  struct FetchCtx fc;
  struct Header *hdr = NULL;
  char buf[HUGE_STRING];
  int rc = 0;
  int oldmsgcount = ctx->msgcount;
  anum_t current;
  anum_t first_over = first;
#ifdef USE_HCACHE
  void *hdata = NULL;
#endif

  /* if empty group or nothing to do */
  if (!last || first > last)
    return 0;

  /* init fetch context */
  fc.ctx = ctx;
  fc.first = first;
   fc.last = last;
   fc.restore = restore;
   fc.messages = mutt_mem_calloc(last - first + 1, sizeof(unsigned char));
  if (fc.messages == NULL)
	  return -1;
 #ifdef USE_HCACHE
   fc.hc = hc;
 #endif

  /* fetch list of articles */
  if (NntpListgroup && nntp_data->nserv->hasLISTGROUP && !nntp_data->deleted)
  {
    if (!ctx->quiet)
      mutt_message(_("Fetching list of articles..."));
    if (nntp_data->nserv->hasLISTGROUPrange)
      snprintf(buf, sizeof(buf), "LISTGROUP %s %u-%u\r\n", nntp_data->group, first, last);
    else
      snprintf(buf, sizeof(buf), "LISTGROUP %s\r\n", nntp_data->group);
    rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), NULL, fetch_numbers, &fc);
    if (rc > 0)
    {
      mutt_error("LISTGROUP: %s", buf);
    }
    if (rc == 0)
    {
      for (current = first; current <= last && rc == 0; current++)
      {
        if (fc.messages[current - first])
          continue;

        snprintf(buf, sizeof(buf), "%u", current);
        if (nntp_data->bcache)
        {
          mutt_debug(2, "#1 mutt_bcache_del %s\n", buf);
          mutt_bcache_del(nntp_data->bcache, buf);
        }

#ifdef USE_HCACHE
        if (fc.hc)
        {
          mutt_debug(2, "mutt_hcache_delete %s\n", buf);
          mutt_hcache_delete(fc.hc, buf, strlen(buf));
        }
#endif
      }
    }
  }
  else
  {
    for (current = first; current <= last; current++)
      fc.messages[current - first] = 1;
  }

  /* fetching header from cache or server, or fallback to fetch overview */
  if (!ctx->quiet)
  {
    mutt_progress_init(&fc.progress, _("Fetching message headers..."),
                       MUTT_PROGRESS_MSG, ReadInc, last - first + 1);
  }
  for (current = first; current <= last && rc == 0; current++)
  {
    if (!ctx->quiet)
      mutt_progress_update(&fc.progress, current - first + 1, -1);

#ifdef USE_HCACHE
    snprintf(buf, sizeof(buf), "%u", current);
#endif

    /* delete header from cache that does not exist on server */
    if (!fc.messages[current - first])
      continue;

    /* allocate memory for headers */
    if (ctx->msgcount >= ctx->hdrmax)
      mx_alloc_memory(ctx);

#ifdef USE_HCACHE
    /* try to fetch header from cache */
    hdata = mutt_hcache_fetch(fc.hc, buf, strlen(buf));
    if (hdata)
    {
      mutt_debug(2, "mutt_hcache_fetch %s\n", buf);
      ctx->hdrs[ctx->msgcount] = hdr = mutt_hcache_restore(hdata);
      mutt_hcache_free(fc.hc, &hdata);
      hdr->data = 0;

      /* skip header marked as deleted in cache */
      if (hdr->deleted && !restore)
      {
        mutt_header_free(&hdr);
        if (nntp_data->bcache)
        {
          mutt_debug(2, "#2 mutt_bcache_del %s\n", buf);
          mutt_bcache_del(nntp_data->bcache, buf);
        }
        continue;
      }

      hdr->read = false;
      hdr->old = false;
    }
    else
#endif

        /* don't try to fetch header from removed newsgroup */
        if (nntp_data->deleted)
      continue;

    /* fallback to fetch overview */
    else if (nntp_data->nserv->hasOVER || nntp_data->nserv->hasXOVER)
    {
      if (NntpListgroup && nntp_data->nserv->hasLISTGROUP)
        break;
      else
        continue;
    }

    /* fetch header from server */
    else
    {
      FILE *fp = mutt_file_mkstemp();
      if (!fp)
      {
        mutt_perror("mutt_file_mkstemp() failed!");
        rc = -1;
        break;
      }

      snprintf(buf, sizeof(buf), "HEAD %u\r\n", current);
      rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), NULL, fetch_tempfile, fp);
      if (rc)
      {
        mutt_file_fclose(&fp);
        if (rc < 0)
          break;

        /* invalid response */
        if (mutt_str_strncmp("423", buf, 3) != 0)
        {
          mutt_error("HEAD: %s", buf);
          break;
        }

        /* no such article */
        if (nntp_data->bcache)
        {
          snprintf(buf, sizeof(buf), "%u", current);
          mutt_debug(2, "#3 mutt_bcache_del %s\n", buf);
          mutt_bcache_del(nntp_data->bcache, buf);
        }
        rc = 0;
        continue;
      }

      /* parse header */
      hdr = ctx->hdrs[ctx->msgcount] = mutt_header_new();
      hdr->env = mutt_rfc822_read_header(fp, hdr, 0, 0);
      hdr->received = hdr->date_sent;
      mutt_file_fclose(&fp);
    }

    /* save header in context */
    hdr->index = ctx->msgcount++;
    hdr->read = false;
    hdr->old = false;
    hdr->deleted = false;
    hdr->data = mutt_mem_calloc(1, sizeof(struct NntpHeaderData));
    NHDR(hdr)->article_num = current;
    if (restore)
      hdr->changed = true;
    else
    {
      nntp_article_status(ctx, hdr, NULL, NHDR(hdr)->article_num);
      if (!hdr->read)
        nntp_parse_xref(ctx, hdr);
    }
    if (current > nntp_data->last_loaded)
      nntp_data->last_loaded = current;
    first_over = current + 1;
  }

  if (!NntpListgroup || !nntp_data->nserv->hasLISTGROUP)
    current = first_over;

  /* fetch overview information */
  if (current <= last && rc == 0 && !nntp_data->deleted)
  {
    char *cmd = nntp_data->nserv->hasOVER ? "OVER" : "XOVER";
    snprintf(buf, sizeof(buf), "%s %u-%u\r\n", cmd, current, last);
    rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), NULL, parse_overview_line, &fc);
    if (rc > 0)
    {
      mutt_error("%s: %s", cmd, buf);
    }
  }

  if (ctx->msgcount > oldmsgcount)
    mx_update_context(ctx, ctx->msgcount - oldmsgcount);

  FREE(&fc.messages);
  if (rc != 0)
    return -1;
  mutt_clear_error();
  return 0;
}
