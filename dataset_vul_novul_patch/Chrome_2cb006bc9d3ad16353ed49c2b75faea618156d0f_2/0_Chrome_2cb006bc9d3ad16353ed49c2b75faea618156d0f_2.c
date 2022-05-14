 bool AppCacheBackendImpl::SelectCacheForWorker(
     int host_id, int parent_process_id, int parent_host_id) {
   AppCacheHost* host = GetHost(host_id);
  if (!host)
     return false;
 
  return host->SelectCacheForWorker(parent_process_id, parent_host_id);
 }
