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
 
  // top-level navigations (not iframes). But we sometimes navigate to
  // about:blank to clear a tab, and we want to still allow that.
  // Note: this is known to break POST submissions when crossing process
  // boundaries until http://crbug.com/101395 is fixed.  This is better for
  // security than loading a WebUI, extension or app page in the wrong process.
  // POST requests don't work because this mechanism does not preserve form
  // POST data. We will need to send the request's httpBody data up to the
  // browser process, and issue a special POST navigation in WebKit (via
   if (!frame->parent() && is_content_initiated &&
      !url.SchemeIs(chrome::kAboutScheme)) {
     bool send_referrer = false;

    // All navigations to WebUI URLs or within WebUI-enabled RenderProcesses
    // must be handled by the browser process so that the correct bindings and
    // data sources can be registered.
    // Similarly, navigations to view-source URLs or within ViewSource mode
    // must be handled by the browser process.
    int cumulative_bindings =
        RenderProcess::current()->GetEnabledBindings();
     bool should_fork =
        content::GetContentClient()->HasWebUIScheme(url) ||
        (cumulative_bindings & content::BINDINGS_POLICY_WEB_UI) ||
        url.SchemeIs(chrome::kViewSourceScheme) ||
        frame->isViewSourceModeEnabled();
 
     if (!should_fork) {
      // For now, we skip this for POST submissions.  This is because
      // http://crbug.com/101395 is more likely to cause compatibility issues
      // with hosted apps and extensions than WebUI pages.  We will remove this
      // check when cross-process POST submissions are supported.
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
