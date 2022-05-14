download::DownloadInterruptReason DownloadManagerImpl::BeginDownloadRequest(
    std::unique_ptr<net::URLRequest> url_request,
    ResourceContext* resource_context,
    download::DownloadUrlParameters* params) {
  if (ResourceDispatcherHostImpl::Get()->is_shutdown())
    return download::DOWNLOAD_INTERRUPT_REASON_USER_SHUTDOWN;

  ResourceDispatcherHostImpl::Get()->InitializeURLRequest(
      url_request.get(),
      Referrer(params->referrer(),
               Referrer::NetReferrerPolicyToBlinkReferrerPolicy(
                    params->referrer_policy())),
       true,  // download.
       params->render_process_host_id(), params->render_view_host_routing_id(),
      params->render_frame_host_routing_id(), params->frame_tree_node_id(),
      PREVIEWS_OFF, resource_context);
 
  url_request->set_first_party_url_policy(
      net::URLRequest::UPDATE_FIRST_PARTY_URL_ON_REDIRECT);

  const GURL& url = url_request->original_url();

  const net::URLRequestContext* request_context = url_request->context();
  if (!request_context->job_factory()->IsHandledProtocol(url.scheme())) {
    DVLOG(1) << "Download request for unsupported protocol: "
             << url.possibly_invalid_spec();
    return download::DOWNLOAD_INTERRUPT_REASON_NETWORK_INVALID_REQUEST;
  }

  std::unique_ptr<ResourceHandler> handler(
      DownloadResourceHandler::CreateForNewRequest(
          url_request.get(), params->request_origin(),
          params->download_source(), params->follow_cross_origin_redirects()));

  ResourceDispatcherHostImpl::Get()->BeginURLRequest(
      std::move(url_request), std::move(handler), true,  // download
      params->content_initiated(), params->do_not_prompt_for_login(),
      resource_context);
  return download::DOWNLOAD_INTERRUPT_REASON_NONE;
}
