 bool AppCacheBackendImpl::SelectCacheForSharedWorker(
     int host_id, int64 appcache_id) {
   AppCacheHost* host = GetHost(host_id);
  if (!host)
     return false;
 
  return host->SelectCacheForSharedWorker(appcache_id);
 }
