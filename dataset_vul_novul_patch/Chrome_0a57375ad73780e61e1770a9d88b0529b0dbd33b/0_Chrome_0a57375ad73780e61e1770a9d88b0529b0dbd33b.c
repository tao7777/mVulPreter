WebNavigationPolicy RenderViewImpl::decidePolicyForNavigation(
    WebFrame* frame, const WebURLRequest& request, WebNavigationType type,
    const WebNode&, WebNavigationPolicy default_policy, bool is_redirect) {
  if (request.url() != GURL(kSwappedOutURL) &&
      GetContentClient()->renderer()->HandleNavigation(frame, request, type,
                                                       default_policy,
                                                       is_redirect)) {
    return WebKit::WebNavigationPolicyIgnore;
  }

  Referrer referrer(
      GURL(request.httpHeaderField(WebString::fromUTF8("Referer"))),
      GetReferrerPolicyFromRequest(frame, request));

  if (is_swapped_out_) {
    if (request.url() != GURL(kSwappedOutURL)) {
      if (frame->parent() == NULL) {
        OpenURL(frame, request.url(), referrer, default_policy);
        return WebKit::WebNavigationPolicyIgnore;  // Suppress the load here.
      }

      return WebKit::WebNavigationPolicyIgnore;
    }

    return default_policy;
  }

  const GURL& url = request.url();

  bool is_content_initiated =
      DocumentState::FromDataSource(frame->provisionalDataSource())->
          navigation_state()->is_content_initiated();

  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  bool force_swap_due_to_flag =
      command_line.HasSwitch(switches::kEnableStrictSiteIsolation) ||
      command_line.HasSwitch(switches::kSitePerProcess);
  if (force_swap_due_to_flag &&
      !frame->parent() && (is_content_initiated || is_redirect)) {
    WebString origin_str = frame->document().securityOrigin().toString();
    GURL frame_url(origin_str.utf8().data());
    if (!net::RegistryControlledDomainService::SameDomainOrHost(frame_url,
                                                                url) ||
        frame_url.scheme() != url.scheme()) {
      OpenURL(frame, url, referrer, default_policy);
      return WebKit::WebNavigationPolicyIgnore;
    }
  }

  if (is_content_initiated) {
    bool browser_handles_request =
        renderer_preferences_.browser_handles_non_local_top_level_requests &&
        IsNonLocalTopLevelNavigation(url, frame, type);
    if (!browser_handles_request) {
      browser_handles_request =
          renderer_preferences_.browser_handles_all_top_level_requests &&
          IsTopLevelNavigation(frame);
    }

    if (browser_handles_request) {
      page_id_ = -1;
      last_page_id_sent_to_browser_ = -1;
      OpenURL(frame, url, referrer, default_policy);
      return WebKit::WebNavigationPolicyIgnore;  // Suppress the load here.
    }
  }

  GURL old_url(frame->dataSource()->request().url());

  if (!frame->parent() && is_content_initiated &&
       !url.SchemeIs(chrome::kAboutScheme)) {
     bool send_referrer = false;
 
    // All navigations to or from WebUI URLs or within WebUI-enabled
    // RenderProcesses must be handled by the browser process so that the
    // correct bindings and data sources can be registered.
     int cumulative_bindings = RenderProcess::current()->GetEnabledBindings();
     bool is_initial_navigation = page_id_ == -1;
    bool should_fork = HasWebUIScheme(url) || HasWebUIScheme(old_url) ||
         (cumulative_bindings & BINDINGS_POLICY_WEB_UI) ||
         url.SchemeIs(chrome::kViewSourceScheme) ||
         (frame->isViewSourceModeEnabled() &&
            type != WebKit::WebNavigationTypeReload);

    if (!should_fork && url.SchemeIs(chrome::kFileScheme)) {
      GURL source_url(old_url);
      if (is_initial_navigation && source_url.is_empty() && frame->opener())
        source_url = frame->opener()->top()->document().url();
      DCHECK(!source_url.is_empty());
      should_fork = !source_url.SchemeIs(chrome::kFileScheme);
    }

    if (!should_fork) {
      should_fork = GetContentClient()->renderer()->ShouldFork(
          frame, url, request.httpMethod().utf8(), is_initial_navigation,
          &send_referrer);
    }

    if (should_fork) {
      OpenURL(
          frame, url, send_referrer ? referrer : Referrer(), default_policy);
      return WebKit::WebNavigationPolicyIgnore;  // Suppress the load here.
    }
  }

  bool is_fork =
      old_url == GURL(chrome::kAboutBlankURL) &&
      historyBackListCount() < 1 &&
      historyForwardListCount() < 1 &&
      frame->opener() == NULL &&
      frame->parent() == NULL &&
      is_content_initiated &&
      default_policy == WebKit::WebNavigationPolicyCurrentTab &&
      type == WebKit::WebNavigationTypeOther;

  if (is_fork) {
    OpenURL(frame, url, Referrer(), default_policy);
    return WebKit::WebNavigationPolicyIgnore;
  }

  return default_policy;
}
