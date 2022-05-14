 bool AppCacheBackendImpl::SelectCacheForWorker(
     int host_id, int parent_process_id, int parent_host_id) {
   AppCacheHost* host = GetHost(host_id);
  if (!host || host->was_select_cache_called())
     return false;
 
  host->SelectCacheForWorker(parent_process_id, parent_host_id);
  return true;
 }
