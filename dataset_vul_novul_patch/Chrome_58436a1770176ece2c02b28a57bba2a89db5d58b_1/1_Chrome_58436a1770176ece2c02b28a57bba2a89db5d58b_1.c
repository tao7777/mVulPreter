WebNavigationPolicy RenderViewImpl::decidePolicyForNavigation(
    WebFrame* frame, const WebURLRequest& request, WebNavigationType type,
    const WebNode&, WebNavigationPolicy default_policy, bool is_redirect) {
   if (is_swapped_out_) {
    if (request.url() != GURL("about:swappedout"))
       return WebKit::WebNavigationPolicyIgnore;
 
     return default_policy;
   }
 
  const GURL& url = request.url();

  bool is_content_initiated =
      DocumentState::FromDataSource(frame->provisionalDataSource())->
          navigation_state()->is_content_initiated();

  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kEnableStrictSiteIsolation) &&
      !frame->parent() && (is_content_initiated || is_redirect)) {
    WebString origin_str = frame->document().securityOrigin().toString();
    GURL frame_url(origin_str.utf8().data());
    if (frame_url.GetOrigin() != url.GetOrigin()) {
      Referrer referrer(
          GURL(request.httpHeaderField(WebString::fromUTF8("Referer"))),
          GetReferrerPolicyFromRequest(request));
      OpenURL(frame, url, referrer, default_policy);
      return WebKit::WebNavigationPolicyIgnore;
    }
  }

  if (is_content_initiated) {
    bool browser_handles_top_level_requests =
        renderer_preferences_.browser_handles_top_level_requests &&
        IsNonLocalTopLevelNavigation(url, frame, type);
    if (browser_handles_top_level_requests ||
        renderer_preferences_.browser_handles_all_requests) {
      Referrer referrer(
          GURL(request.httpHeaderField(WebString::fromUTF8("Referer"))),
          GetReferrerPolicyFromRequest(request));
      page_id_ = -1;
      last_page_id_sent_to_browser_ = -1;
      OpenURL(frame, url, referrer, default_policy);
      return WebKit::WebNavigationPolicyIgnore;  // Suppress the load here.
    }
  }

  if (!frame->parent() && is_content_initiated &&
      !url.SchemeIs(chrome::kAboutScheme)) {
    bool send_referrer = false;

    int cumulative_bindings =
        RenderProcess::current()->GetEnabledBindings();
    bool should_fork =
        content::GetContentClient()->HasWebUIScheme(url) ||
        (cumulative_bindings & content::BINDINGS_POLICY_WEB_UI) ||
        url.SchemeIs(chrome::kViewSourceScheme) ||
        frame->isViewSourceModeEnabled();

    if (!should_fork) {
      if (request.httpMethod() == "GET") {
        bool is_initial_navigation = page_id_ == -1;
        should_fork = content::GetContentClient()->renderer()->ShouldFork(
            frame, url, is_initial_navigation, &send_referrer);
      }
    }

    if (should_fork) {
      Referrer referrer(
          GURL(request.httpHeaderField(WebString::fromUTF8("Referer"))),
          GetReferrerPolicyFromRequest(request));
      OpenURL(
          frame, url, send_referrer ? referrer : Referrer(), default_policy);
      return WebKit::WebNavigationPolicyIgnore;  // Suppress the load here.
    }
  }

  GURL old_url(frame->dataSource()->request().url());

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
