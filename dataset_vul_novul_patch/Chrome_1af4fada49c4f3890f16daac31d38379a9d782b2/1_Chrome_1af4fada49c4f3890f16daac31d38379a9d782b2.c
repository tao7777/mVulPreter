void ResourceDispatcherHostImpl::BeginRequest(
    int request_id,
    const ResourceHostMsg_Request& request_data,
    IPC::Message* sync_result,  // only valid for sync
    int route_id) {
   int process_type = filter_->process_type();
   int child_id = filter_->child_id();
 
   if (IsBrowserSideNavigationEnabled() &&
       IsResourceTypeFrame(request_data.resource_type) &&
      !request_data.url.SchemeIs(url::kBlobScheme)) {
    bad_message::ReceivedBadMessage(filter_, bad_message::RDH_INVALID_URL);
    return;
  }

  if (request_data.priority < net::MINIMUM_PRIORITY ||
      request_data.priority > net::MAXIMUM_PRIORITY) {
    bad_message::ReceivedBadMessage(filter_, bad_message::RDH_INVALID_PRIORITY);
    return;
  }

  char url_buf[128];
  base::strlcpy(url_buf, request_data.url.spec().c_str(), arraysize(url_buf));
  base::debug::Alias(url_buf);

  LoaderMap::iterator it = pending_loaders_.find(
      GlobalRequestID(request_data.transferred_request_child_id,
                      request_data.transferred_request_request_id));
  if (it != pending_loaders_.end()) {
    if (it->second->is_transferring()) {
      ResourceLoader* deferred_loader = it->second.get();
      UpdateRequestForTransfer(child_id, route_id, request_id,
                               request_data, it);
      deferred_loader->CompleteTransfer();
    } else {
      bad_message::ReceivedBadMessage(
          filter_, bad_message::RDH_REQUEST_NOT_TRANSFERRING);
    }
    return;
  }

  ResourceContext* resource_context = NULL;
  net::URLRequestContext* request_context = NULL;
  filter_->GetContexts(request_data.resource_type, request_data.origin_pid,
                       &resource_context, &request_context);
  CHECK(ContainsKey(active_resource_contexts_, resource_context));

  net::HttpRequestHeaders headers;
  headers.AddHeadersFromString(request_data.headers);

  if (is_shutdown_ ||
      !ShouldServiceRequest(process_type, child_id, request_data, headers,
                            filter_, resource_context)) {
    AbortRequestBeforeItStarts(filter_, sync_result, request_id);
    return;
  }

  if (delegate_ && !delegate_->ShouldBeginRequest(request_data.method,
                                                  request_data.url,
                                                  request_data.resource_type,
                                                  resource_context)) {
    AbortRequestBeforeItStarts(filter_, sync_result, request_id);
    return;
  }

  scoped_ptr<net::URLRequest> new_request = request_context->CreateRequest(
      request_data.url, request_data.priority, NULL);

  new_request->set_method(request_data.method);
  new_request->set_first_party_for_cookies(
      request_data.first_party_for_cookies);
  new_request->set_initiator(request_data.request_initiator);

  if (request_data.resource_type == RESOURCE_TYPE_MAIN_FRAME) {
    new_request->set_first_party_url_policy(
        net::URLRequest::UPDATE_FIRST_PARTY_URL_ON_REDIRECT);
  }

  const Referrer referrer(request_data.referrer, request_data.referrer_policy);
  SetReferrerForRequest(new_request.get(), referrer);

  new_request->SetExtraRequestHeaders(headers);

  storage::BlobStorageContext* blob_context =
      GetBlobStorageContext(filter_->blob_storage_context());
  if (request_data.request_body.get()) {
    if (blob_context) {
      AttachRequestBodyBlobDataHandles(
          request_data.request_body.get(),
          blob_context);
    }
    new_request->set_upload(UploadDataStreamBuilder::Build(
        request_data.request_body.get(),
        blob_context,
        filter_->file_system_context(),
        BrowserThread::GetMessageLoopProxyForThread(BrowserThread::FILE)
            .get()));
  }

  bool allow_download = request_data.allow_download &&
      IsResourceTypeFrame(request_data.resource_type);
  bool do_not_prompt_for_login = request_data.do_not_prompt_for_login;
  bool is_sync_load = sync_result != NULL;

  ChildProcessSecurityPolicyImpl* policy =
      ChildProcessSecurityPolicyImpl::GetInstance();
  bool report_raw_headers = request_data.report_raw_headers;
  if (report_raw_headers && !policy->CanReadRawCookies(child_id)) {
    VLOG(1) << "Denied unauthorized request for raw headers";
    report_raw_headers = false;
  }
  int load_flags =
      BuildLoadFlagsForRequest(request_data, child_id, is_sync_load);
  if (request_data.resource_type == RESOURCE_TYPE_PREFETCH ||
      request_data.resource_type == RESOURCE_TYPE_FAVICON) {
    do_not_prompt_for_login = true;
  }
  if (request_data.resource_type == RESOURCE_TYPE_IMAGE &&
      HTTP_AUTH_RELATION_BLOCKED_CROSS ==
          HttpAuthRelationTypeOf(request_data.url,
                                 request_data.first_party_for_cookies)) {
    do_not_prompt_for_login = true;
    load_flags |= net::LOAD_DO_NOT_USE_EMBEDDED_IDENTITY;
  }

  bool support_async_revalidation =
      !is_sync_load && async_revalidation_manager_ &&
      AsyncRevalidationManager::QualifiesForAsyncRevalidation(request_data);

  if (support_async_revalidation)
    load_flags |= net::LOAD_SUPPORT_ASYNC_REVALIDATION;

  if (is_sync_load) {
    DCHECK_EQ(request_data.priority, net::MAXIMUM_PRIORITY);
    DCHECK_NE(load_flags & net::LOAD_IGNORE_LIMITS, 0);
  } else {
    DCHECK_EQ(load_flags & net::LOAD_IGNORE_LIMITS, 0);
  }
  new_request->SetLoadFlags(load_flags);

  ResourceRequestInfoImpl* extra_info = new ResourceRequestInfoImpl(
      process_type, child_id, route_id,
      -1,  // frame_tree_node_id
      request_data.origin_pid, request_id, request_data.render_frame_id,
      request_data.is_main_frame, request_data.parent_is_main_frame,
      request_data.resource_type, request_data.transition_type,
      request_data.should_replace_current_entry,
      false,  // is download
      false,  // is stream
      allow_download, request_data.has_user_gesture,
      request_data.enable_load_timing, request_data.enable_upload_progress,
      do_not_prompt_for_login, request_data.referrer_policy,
      request_data.visiblity_state, resource_context, filter_->GetWeakPtr(),
      report_raw_headers, !is_sync_load,
      IsUsingLoFi(request_data.lofi_state, delegate_, *new_request,
                  resource_context,
                  request_data.resource_type == RESOURCE_TYPE_MAIN_FRAME),
      support_async_revalidation ? request_data.headers : std::string());
  extra_info->AssociateWithRequest(new_request.get());

  if (new_request->url().SchemeIs(url::kBlobScheme)) {
    storage::BlobProtocolHandler::SetRequestedBlobDataHandle(
        new_request.get(),
        filter_->blob_storage_context()->context()->GetBlobDataFromPublicURL(
            new_request->url()));
  }

  const bool should_skip_service_worker =
      request_data.skip_service_worker || is_sync_load;
  ServiceWorkerRequestHandler::InitializeHandler(
      new_request.get(), filter_->service_worker_context(), blob_context,
      child_id, request_data.service_worker_provider_id,
      should_skip_service_worker,
      request_data.fetch_request_mode, request_data.fetch_credentials_mode,
      request_data.fetch_redirect_mode, request_data.resource_type,
      request_data.fetch_request_context_type, request_data.fetch_frame_type,
      request_data.request_body);

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableExperimentalWebPlatformFeatures)) {
    ForeignFetchRequestHandler::InitializeHandler(
        new_request.get(), filter_->service_worker_context(), blob_context,
        child_id, request_data.service_worker_provider_id,
        should_skip_service_worker,
        request_data.fetch_request_mode, request_data.fetch_credentials_mode,
        request_data.fetch_redirect_mode, request_data.resource_type,
        request_data.fetch_request_context_type, request_data.fetch_frame_type,
        request_data.request_body);
  }

  AppCacheInterceptor::SetExtraRequestInfo(
      new_request.get(), filter_->appcache_service(), child_id,
      request_data.appcache_host_id, request_data.resource_type,
      request_data.should_reset_appcache);

  scoped_ptr<ResourceHandler> handler(
       CreateResourceHandler(
           new_request.get(),
           request_data, sync_result, route_id, process_type, child_id,
           resource_context));

  if (handler)
    BeginRequestInternal(std::move(new_request), std::move(handler));
}
