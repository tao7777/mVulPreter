void AppCacheHost::SelectCacheForWorker(int parent_process_id,
                                         int parent_host_id) {
   DCHECK(pending_start_update_callback_.is_null() &&
          pending_swap_cache_callback_.is_null() &&
          pending_get_status_callback_.is_null() &&
         !is_selection_pending() && !was_select_cache_called_);
 
   was_select_cache_called_ = true;
   parent_process_id_ = parent_process_id;
   parent_host_id_ = parent_host_id;
   FinishCacheSelection(NULL, NULL);
 }
