 bool AppCacheBackendImpl::SelectCacheForSharedWorker(
     int host_id, int64 appcache_id) {
   AppCacheHost* host = GetHost(host_id);
  if (!host || host->was_select_cache_called())
     return false;
 
  host->SelectCacheForSharedWorker(appcache_id);
  return true;
 }
