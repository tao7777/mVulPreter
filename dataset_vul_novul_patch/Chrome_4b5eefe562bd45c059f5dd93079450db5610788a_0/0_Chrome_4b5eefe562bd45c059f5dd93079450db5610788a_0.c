 void AppCacheDispatcherHost::OnChannelConnected(int32 peer_pid) {
  if (appcache_service_.get()) {
    backend_impl_.Initialize(
         appcache_service_.get(), &frontend_proxy_, process_id_);
     get_status_callback_ =
         base::Bind(&AppCacheDispatcherHost::GetStatusCallback,
                    weak_factory_.GetWeakPtr());
     start_update_callback_ =
         base::Bind(&AppCacheDispatcherHost::StartUpdateCallback,
                    weak_factory_.GetWeakPtr());
     swap_cache_callback_ =
         base::Bind(&AppCacheDispatcherHost::SwapCacheCallback,
                    weak_factory_.GetWeakPtr());
   }
 }
