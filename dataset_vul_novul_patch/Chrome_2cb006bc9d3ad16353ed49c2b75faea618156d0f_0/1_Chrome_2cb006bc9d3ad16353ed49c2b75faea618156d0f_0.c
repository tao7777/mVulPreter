bool AppCacheBackendImpl::SelectCache(
    int host_id,
    const GURL& document_url,
     const int64 cache_document_was_loaded_from,
     const GURL& manifest_url) {
   AppCacheHost* host = GetHost(host_id);
  if (!host || host->was_select_cache_called())
     return false;
 
  host->SelectCache(document_url, cache_document_was_loaded_from,
                     manifest_url);
  return true;
 }
