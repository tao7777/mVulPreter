void ExternalProtocolHandler::LaunchUrlWithDelegate(
    const GURL& url,
    int render_process_host_id,
    int render_view_routing_id,
    ui::PageTransition page_transition,
    bool has_user_gesture,
    Delegate* delegate) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
 
  // TODO(mgiuca): This essentially amounts to "remove illegal characters from
  // the URL", something that probably should be done by the GURL constructor
  // itself. The GURL constructor does do it in some cases (e.g., mailto) but
  // not in general. https://crbug.com/788244.
   std::string escaped_url_string = net::EscapeExternalHandlerValue(url.spec());
   GURL escaped_url(escaped_url_string);
 
  content::WebContents* web_contents = tab_util::GetWebContentsByID(
      render_process_host_id, render_view_routing_id);
  Profile* profile = nullptr;
  if (web_contents)  // Maybe NULL during testing.
    profile = Profile::FromBrowserContext(web_contents->GetBrowserContext());
  BlockState block_state =
      GetBlockStateWithDelegate(escaped_url.scheme(), delegate, profile);
  if (block_state == BLOCK) {
    if (delegate)
      delegate->BlockRequest();
    return;
  }

  g_accept_requests = false;

   shell_integration::DefaultWebClientWorkerCallback callback = base::Bind(
      &OnDefaultProtocolClientWorkerFinished, escaped_url,
      render_process_host_id, render_view_routing_id, block_state == UNKNOWN,
      page_transition, has_user_gesture, delegate);
 
  CreateShellWorker(callback, escaped_url.scheme(), delegate)
      ->StartCheckIsDefault();
}
