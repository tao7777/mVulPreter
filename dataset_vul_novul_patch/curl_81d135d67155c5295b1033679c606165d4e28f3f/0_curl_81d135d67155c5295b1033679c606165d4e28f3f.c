CURLcode Curl_close(struct Curl_easy *data)
{
  struct Curl_multi *m;

  if(!data)
    return CURLE_OK;

  Curl_expire_clear(data); /* shut off timers */

  m = data->multi;
  if(m)
    /* This handle is still part of a multi handle, take care of this first
        and detach this handle from there. */
     curl_multi_remove_handle(data->multi, data);
 
  if(data->multi_easy) {
     /* when curl_easy_perform() is used, it creates its own multi handle to
        use and this is the one */
     curl_multi_cleanup(data->multi_easy);
    data->multi_easy = NULL;
  }
 
   /* Destroy the timeout list that is held in the easy handle. It is
      /normally/ done by curl_multi_remove_handle() but this is "just in
     case" */
  Curl_llist_destroy(&data->state.timeoutlist, NULL);

  data->magic = 0; /* force a clear AFTER the possibly enforced removal from
                      the multi handle, since that function uses the magic
                      field! */

  if(data->state.rangestringalloc)
    free(data->state.range);

  /* freed here just in case DONE wasn't called */
  Curl_free_request_state(data);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(data);
  Curl_safefree(data->state.first_host);
  Curl_safefree(data->state.scratch);
  Curl_ssl_free_certinfo(data);

  /* Cleanup possible redirect junk */
  free(data->req.newurl);
  data->req.newurl = NULL;

  if(data->change.referer_alloc) {
    Curl_safefree(data->change.referer);
    data->change.referer_alloc = FALSE;
  }
  data->change.referer = NULL;

  Curl_up_free(data);
  Curl_safefree(data->state.buffer);
  Curl_safefree(data->state.headerbuff);
  Curl_safefree(data->state.ulbuf);
  Curl_flush_cookies(data, 1);
  Curl_digest_cleanup(data);
  Curl_safefree(data->info.contenttype);
  Curl_safefree(data->info.wouldredirect);

  /* this destroys the channel and we cannot use it anymore after this */
  Curl_resolver_cleanup(data->state.resolver);

  Curl_http2_cleanup_dependencies(data);
  Curl_convert_close(data);

  /* No longer a dirty share, if it exists */
  if(data->share) {
    Curl_share_lock(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE);
    data->share->dirty--;
    Curl_share_unlock(data, CURL_LOCK_DATA_SHARE);
  }

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&data->wildcard);
  Curl_freeset(data);
  free(data);
  return CURLE_OK;
}
