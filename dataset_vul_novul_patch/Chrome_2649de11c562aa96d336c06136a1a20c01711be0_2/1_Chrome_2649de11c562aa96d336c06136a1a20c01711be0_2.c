bool WebRequestPermissions::HideRequest(
    const extensions::InfoMap* extension_info_map,
    const extensions::WebRequestInfo& request) {
  if (request.is_web_view)
    return false;

  if (request.is_pac_request)
    return true;
 
  bool is_request_from_browser = request.render_process_id == -1;
   bool is_request_from_webui_renderer = false;
   if (!is_request_from_browser) {
    if (request.is_web_view)
      return false;

    if (extension_info_map &&
        extension_info_map->process_map().Contains(extensions::kWebStoreAppId,
                                                   request.render_process_id)) {
      return true;
    }

    is_request_from_webui_renderer =
        content::ChildProcessSecurityPolicy::GetInstance()->HasWebUIBindings(
            request.render_process_id);
  }

  return IsSensitiveURL(request.url, is_request_from_browser ||
                                         is_request_from_webui_renderer) ||
         !HasWebRequestScheme(request.url);
}
