void ProfileImplIOData::LazyInitializeInternal(
    ProfileParams* profile_params) const {
  clear_local_state_on_exit_ = profile_params->clear_local_state_on_exit;

  ChromeURLRequestContext* main_context = main_request_context();
  ChromeURLRequestContext* extensions_context = extensions_request_context();
  media_request_context_ = new ChromeURLRequestContext;

  IOThread* const io_thread = profile_params->io_thread;
  IOThread::Globals* const io_thread_globals = io_thread->globals();
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  bool record_mode = chrome::kRecordModeEnabled &&
                     command_line.HasSwitch(switches::kRecordMode);
  bool playback_mode = command_line.HasSwitch(switches::kPlaybackMode);


  ApplyProfileParamsToContext(main_context);
  ApplyProfileParamsToContext(media_request_context_);
  ApplyProfileParamsToContext(extensions_context);

  if (http_server_properties_manager_.get())
    http_server_properties_manager_->InitializeOnIOThread();

  main_context->set_transport_security_state(transport_security_state());
  media_request_context_->set_transport_security_state(
      transport_security_state());
  extensions_context->set_transport_security_state(transport_security_state());

  main_context->set_net_log(io_thread->net_log());
  media_request_context_->set_net_log(io_thread->net_log());
  extensions_context->set_net_log(io_thread->net_log());

  main_context->set_network_delegate(network_delegate());
  media_request_context_->set_network_delegate(network_delegate());

  main_context->set_http_server_properties(http_server_properties());
  media_request_context_->set_http_server_properties(http_server_properties());

  main_context->set_host_resolver(
      io_thread_globals->host_resolver.get());
  media_request_context_->set_host_resolver(
      io_thread_globals->host_resolver.get());
  main_context->set_cert_verifier(
      io_thread_globals->cert_verifier.get());
  media_request_context_->set_cert_verifier(
      io_thread_globals->cert_verifier.get());
  main_context->set_http_auth_handler_factory(
      io_thread_globals->http_auth_handler_factory.get());
  media_request_context_->set_http_auth_handler_factory(
      io_thread_globals->http_auth_handler_factory.get());

  main_context->set_fraudulent_certificate_reporter(
      fraudulent_certificate_reporter());
  media_request_context_->set_fraudulent_certificate_reporter(
      fraudulent_certificate_reporter());

  main_context->set_proxy_service(proxy_service());
  media_request_context_->set_proxy_service(proxy_service());

  scoped_refptr<net::CookieStore> cookie_store = NULL;
  net::OriginBoundCertService* origin_bound_cert_service = NULL;
  if (record_mode || playback_mode) {
    cookie_store = new net::CookieMonster(
        NULL, profile_params->cookie_monster_delegate);
    origin_bound_cert_service = new net::OriginBoundCertService(
        new net::DefaultOriginBoundCertStore(NULL));
  }

  if (!cookie_store) {
    DCHECK(!lazy_params_->cookie_path.empty());

    scoped_refptr<SQLitePersistentCookieStore> cookie_db =
        new SQLitePersistentCookieStore(
            lazy_params_->cookie_path,
            lazy_params_->restore_old_session_cookies);
    cookie_db->SetClearLocalStateOnExit(
        profile_params->clear_local_state_on_exit);
    cookie_store =
        new net::CookieMonster(cookie_db.get(),
                               profile_params->cookie_monster_delegate);
    if (command_line.HasSwitch(switches::kEnableRestoreSessionState))
      cookie_store->GetCookieMonster()->SetPersistSessionCookies(true);
  }

  net::CookieMonster* extensions_cookie_store =
      new net::CookieMonster(
          new SQLitePersistentCookieStore(
              lazy_params_->extensions_cookie_path,
              lazy_params_->restore_old_session_cookies), NULL);
  const char* schemes[] = {chrome::kChromeDevToolsScheme,
                           chrome::kExtensionScheme};
  extensions_cookie_store->SetCookieableSchemes(schemes, 2);

  main_context->set_cookie_store(cookie_store);
  media_request_context_->set_cookie_store(cookie_store);
  extensions_context->set_cookie_store(extensions_cookie_store);

  if (!origin_bound_cert_service) {
    DCHECK(!lazy_params_->origin_bound_cert_path.empty());

    scoped_refptr<SQLiteOriginBoundCertStore> origin_bound_cert_db =
        new SQLiteOriginBoundCertStore(lazy_params_->origin_bound_cert_path);
    origin_bound_cert_db->SetClearLocalStateOnExit(
        profile_params->clear_local_state_on_exit);
    origin_bound_cert_service = new net::OriginBoundCertService(
        new net::DefaultOriginBoundCertStore(origin_bound_cert_db.get()));
  }

  set_origin_bound_cert_service(origin_bound_cert_service);
  main_context->set_origin_bound_cert_service(origin_bound_cert_service);
  media_request_context_->set_origin_bound_cert_service(
      origin_bound_cert_service);

  net::HttpCache::DefaultBackend* main_backend =
      new net::HttpCache::DefaultBackend(
          net::DISK_CACHE,
          lazy_params_->cache_path,
          lazy_params_->cache_max_size,
          BrowserThread::GetMessageLoopProxyForThread(BrowserThread::CACHE));
  net::HttpCache* main_cache = new net::HttpCache(
      main_context->host_resolver(),
      main_context->cert_verifier(),
      main_context->origin_bound_cert_service(),
      main_context->transport_security_state(),
      main_context->proxy_service(),
      "", // pass empty ssl_session_cache_shard to share the SSL session cache
      main_context->ssl_config_service(),
      main_context->http_auth_handler_factory(),
      main_context->network_delegate(),
      main_context->http_server_properties(),
      main_context->net_log(),
      main_backend);

  net::HttpCache::DefaultBackend* media_backend =
      new net::HttpCache::DefaultBackend(
          net::MEDIA_CACHE, lazy_params_->media_cache_path,
          lazy_params_->media_cache_max_size,
          BrowserThread::GetMessageLoopProxyForThread(BrowserThread::CACHE));
  net::HttpNetworkSession* main_network_session = main_cache->GetSession();
  net::HttpCache* media_cache =
      new net::HttpCache(main_network_session, media_backend);

  if (record_mode || playback_mode) {
    main_cache->set_mode(
        record_mode ? net::HttpCache::RECORD : net::HttpCache::PLAYBACK);
  }

  main_http_factory_.reset(main_cache);
  media_http_factory_.reset(media_cache);
  main_context->set_http_transaction_factory(main_cache);
  media_request_context_->set_http_transaction_factory(media_cache);

   ftp_factory_.reset(
       new net::FtpNetworkLayer(io_thread_globals->host_resolver.get()));
   main_context->set_ftp_transaction_factory(ftp_factory_.get());
  media_request_context_->set_ftp_transaction_factory(ftp_factory_.get());
 
   main_context->set_chrome_url_data_manager_backend(
       chrome_url_data_manager_backend());

  main_context->set_job_factory(job_factory());
  media_request_context_->set_job_factory(job_factory());
  extensions_context->set_job_factory(job_factory());

  job_factory()->AddInterceptor(
      new chrome_browser_net::ConnectInterceptor(predictor_.get()));

  lazy_params_.reset();
}
