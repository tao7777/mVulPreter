void AppCacheHost::SelectCacheForSharedWorker(int64 appcache_id) {
   DCHECK(pending_start_update_callback_.is_null() &&
          pending_swap_cache_callback_.is_null() &&
          pending_get_status_callback_.is_null() &&
         !is_selection_pending() && !was_select_cache_called_);
 
   was_select_cache_called_ = true;
   if (appcache_id != kAppCacheNoCacheId) {
     LoadSelectedCache(appcache_id);
    return;
   }
   FinishCacheSelection(NULL, NULL);
 }
