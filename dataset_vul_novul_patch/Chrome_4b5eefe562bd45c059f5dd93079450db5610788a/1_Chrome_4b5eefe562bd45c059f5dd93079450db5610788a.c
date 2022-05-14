AppCacheDispatcherHost::AppCacheDispatcherHost(
    ChromeAppCacheService* appcache_service,
    int process_id)
     : BrowserMessageFilter(AppCacheMsgStart),
       appcache_service_(appcache_service),
       frontend_proxy_(this),
      process_id_(process_id) {
 }
