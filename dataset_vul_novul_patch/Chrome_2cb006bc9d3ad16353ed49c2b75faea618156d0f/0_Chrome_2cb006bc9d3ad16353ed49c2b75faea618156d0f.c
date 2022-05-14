 bool AppCacheBackendImpl::MarkAsForeignEntry(
    int host_id,
    const GURL& document_url,
    int64 cache_document_was_loaded_from) {
  AppCacheHost* host = GetHost(host_id);
   if (!host)
     return false;
 
  return host->MarkAsForeignEntry(document_url, cache_document_was_loaded_from);
 }
