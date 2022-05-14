 void AppCacheDispatcherHost::OnChannelConnected(int32 peer_pid) {
  if (appcache_service_.get()) {
    backend_impl_.Initialize(
         appcache_service_.get(), &frontend_proxy_, process_id_);
     get_status_callback_ =
         base::Bind(&AppCacheDispatcherHost::GetStatusCallback,
                   base::Unretained(this));
     start_update_callback_ =
         base::Bind(&AppCacheDispatcherHost::StartUpdateCallback,
                   base::Unretained(this));
     swap_cache_callback_ =
         base::Bind(&AppCacheDispatcherHost::SwapCacheCallback,
                   base::Unretained(this));
   }
 }
