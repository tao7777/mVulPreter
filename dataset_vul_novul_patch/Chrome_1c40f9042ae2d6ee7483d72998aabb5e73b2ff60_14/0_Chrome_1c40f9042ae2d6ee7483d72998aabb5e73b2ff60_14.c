bool ResourceLoader::WillFollowRedirect(
    const WebURL& new_url,
    const WebURL& new_site_for_cookies,
    const WebString& new_referrer,
    WebReferrerPolicy new_referrer_policy,
    const WebString& new_method,
    const WebURLResponse& passed_redirect_response,
    bool& report_raw_headers) {
  DCHECK(!passed_redirect_response.IsNull());

  if (is_cache_aware_loading_activated_) {
    HandleError(
        ResourceError::CacheMissError(resource_->LastResourceRequest().Url()));
    return false;
  }

  const ResourceRequest& last_request = resource_->LastResourceRequest();
  ResourceRequest new_request(new_url);
  new_request.SetSiteForCookies(new_site_for_cookies);
  new_request.SetDownloadToFile(last_request.DownloadToFile());
  new_request.SetUseStreamOnResponse(last_request.UseStreamOnResponse());
  new_request.SetRequestContext(last_request.GetRequestContext());
  new_request.SetFrameType(last_request.GetFrameType());
  new_request.SetServiceWorkerMode(
      passed_redirect_response.WasFetchedViaServiceWorker()
          ? WebURLRequest::ServiceWorkerMode::kAll
          : WebURLRequest::ServiceWorkerMode::kNone);
  new_request.SetShouldResetAppCache(last_request.ShouldResetAppCache());
  new_request.SetFetchRequestMode(last_request.GetFetchRequestMode());
  new_request.SetFetchCredentialsMode(last_request.GetFetchCredentialsMode());
  new_request.SetKeepalive(last_request.GetKeepalive());
  String referrer =
      new_referrer.IsEmpty() ? Referrer::NoReferrer() : String(new_referrer);
  new_request.SetHTTPReferrer(
      Referrer(referrer, static_cast<ReferrerPolicy>(new_referrer_policy)));
  new_request.SetPriority(last_request.Priority());
  new_request.SetHTTPMethod(new_method);
  if (new_request.HttpMethod() == last_request.HttpMethod())
    new_request.SetHTTPBody(last_request.HttpBody());
  new_request.SetCheckForBrowserSideNavigation(
      last_request.CheckForBrowserSideNavigation());

  Resource::Type resource_type = resource_->GetType();

  const ResourceRequest& initial_request = resource_->GetResourceRequest();
  WebURLRequest::RequestContext request_context =
      initial_request.GetRequestContext();
  WebURLRequest::FrameType frame_type = initial_request.GetFrameType();
  WebURLRequest::FetchRequestMode fetch_request_mode =
      initial_request.GetFetchRequestMode();
  WebURLRequest::FetchCredentialsMode fetch_credentials_mode =
      initial_request.GetFetchCredentialsMode();

  const ResourceLoaderOptions& options = resource_->Options();

  const ResourceResponse& redirect_response(
      passed_redirect_response.ToResourceResponse());

  new_request.SetRedirectStatus(
      ResourceRequest::RedirectStatus::kFollowedRedirect);

  if (!IsManualRedirectFetchRequest(initial_request)) {
    bool unused_preload = resource_->IsUnusedPreload();

    SecurityViolationReportingPolicy reporting_policy =
        unused_preload ? SecurityViolationReportingPolicy::kSuppressReporting
                       : SecurityViolationReportingPolicy::kReport;

    Context().CheckCSPForRequest(
        request_context, new_url, options, reporting_policy,
        ResourceRequest::RedirectStatus::kFollowedRedirect);

    ResourceRequestBlockedReason blocked_reason = Context().CanRequest(
        resource_type, new_request, new_url, options, reporting_policy,
        FetchParameters::kUseDefaultOriginRestrictionForType,
        ResourceRequest::RedirectStatus::kFollowedRedirect);
    if (blocked_reason != ResourceRequestBlockedReason::kNone) {
      CancelForRedirectAccessCheckError(new_url, blocked_reason);
      return false;
    }

    if (options.cors_handling_by_resource_fetcher ==
            kEnableCORSHandlingByResourceFetcher &&
        fetch_request_mode == WebURLRequest::kFetchRequestModeCORS) {
      RefPtr<SecurityOrigin> source_origin = options.security_origin;
      if (!source_origin.get())
        source_origin = Context().GetSecurityOrigin();
      WebSecurityOrigin source_web_origin(source_origin.get());
      WrappedResourceRequest new_request_wrapper(new_request);
      WebString cors_error_msg;
      if (!WebCORS::HandleRedirect(
              source_web_origin, new_request_wrapper, redirect_response.Url(),
              redirect_response.HttpStatusCode(),
              redirect_response.HttpHeaderFields(), fetch_credentials_mode,
              resource_->MutableOptions(), cors_error_msg)) {
        resource_->SetCORSStatus(CORSStatus::kFailed);

        if (!unused_preload) {
          Context().AddErrorConsoleMessage(cors_error_msg,
                                           FetchContext::kJSSource);
        }

        CancelForRedirectAccessCheckError(new_url,
                                          ResourceRequestBlockedReason::kOther);
        return false;
      }

      source_origin = source_web_origin;
    }
    if (resource_type == Resource::kImage &&
        fetcher_->ShouldDeferImageLoad(new_url)) {
      CancelForRedirectAccessCheckError(new_url,
                                        ResourceRequestBlockedReason::kOther);
      return false;
    }
  }

  bool cross_origin =
      !SecurityOrigin::AreSameSchemeHostPort(redirect_response.Url(), new_url);
  fetcher_->RecordResourceTimingOnRedirect(resource_.Get(), redirect_response,
                                           cross_origin);

  if (options.cors_handling_by_resource_fetcher ==
          kEnableCORSHandlingByResourceFetcher &&
      fetch_request_mode == WebURLRequest::kFetchRequestModeCORS) {
    bool allow_stored_credentials = false;
    switch (fetch_credentials_mode) {
      case WebURLRequest::kFetchCredentialsModeOmit:
        break;
      case WebURLRequest::kFetchCredentialsModeSameOrigin:
        allow_stored_credentials = !options.cors_flag;
        break;
      case WebURLRequest::kFetchCredentialsModeInclude:
      case WebURLRequest::kFetchCredentialsModePassword:
        allow_stored_credentials = true;
        break;
    }
    new_request.SetAllowStoredCredentials(allow_stored_credentials);
  }


   Context().PrepareRequest(new_request,
                            FetchContext::RedirectType::kForRedirect);
   Context().DispatchWillSendRequest(resource_->Identifier(), new_request,
                                    redirect_response, resource_->GetType(),
                                    options.initiator_info);
 
  DCHECK(KURL(new_site_for_cookies) == new_request.SiteForCookies());

  DCHECK_EQ(new_request.GetRequestContext(), request_context);
  DCHECK_EQ(new_request.GetFrameType(), frame_type);
  DCHECK_EQ(new_request.GetFetchRequestMode(), fetch_request_mode);
  DCHECK_EQ(new_request.GetFetchCredentialsMode(), fetch_credentials_mode);

  if (new_request.Url() != KURL(new_url)) {
    CancelForRedirectAccessCheckError(new_request.Url(),
                                      ResourceRequestBlockedReason::kOther);
    return false;
  }

  if (!resource_->WillFollowRedirect(new_request, redirect_response)) {
    CancelForRedirectAccessCheckError(new_request.Url(),
                                      ResourceRequestBlockedReason::kOther);
    return false;
  }

  report_raw_headers = new_request.ReportRawHeaders();

  return true;
}
