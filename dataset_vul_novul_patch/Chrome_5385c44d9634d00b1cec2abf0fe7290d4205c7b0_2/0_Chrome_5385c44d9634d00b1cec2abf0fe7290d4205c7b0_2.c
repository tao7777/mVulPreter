ResourceDispatcherHostImpl::ResourceDispatcherHostImpl()
     : download_file_manager_(new DownloadFileManager(NULL)),
       save_file_manager_(new SaveFileManager()),
       request_id_(-1),
       is_shutdown_(false),
       max_outstanding_requests_cost_per_process_(
           kMaxOutstandingRequestsCostPerProcess),
      filter_(NULL),
      delegate_(NULL),
      allow_cross_origin_auth_prompt_(false) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!g_resource_dispatcher_host);
  g_resource_dispatcher_host = this;

  GetContentClient()->browser()->ResourceDispatcherHostCreated();

  ANNOTATE_BENIGN_RACE(
      &last_user_gesture_time_,
      "We don't care about the precise value, see http://crbug.com/92889");

  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&appcache::AppCacheInterceptor::EnsureRegistered));

  update_load_states_timer_.reset(
      new base::RepeatingTimer<ResourceDispatcherHostImpl>());
}
