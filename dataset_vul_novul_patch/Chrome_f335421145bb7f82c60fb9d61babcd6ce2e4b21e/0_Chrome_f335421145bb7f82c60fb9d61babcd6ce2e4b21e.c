 ExtensionFunction* ExtensionFunctionDispatcher::CreateExtensionFunction(
     const ExtensionHostMsg_Request_Params& params,
    const Extension* extension,
    int requesting_process_id,
    const extensions::ProcessMap& process_map,
    extensions::ExtensionAPI* api,
    void* profile,
    IPC::Sender* ipc_sender,
    RenderViewHost* render_view_host,
    int routing_id) {
  if (!extension) {
    LOG(ERROR) << "Specified extension does not exist.";
    SendAccessDenied(ipc_sender, routing_id, params.request_id);
     return NULL;
   }
 
  // Most hosted apps can't call APIs.
  bool allowed = true;
  if (extension->is_hosted_app())
      allowed = AllowHostedAppAPICall(*extension, params.source_url,
                                      params.name);

  // Privileged APIs can only be called from the process the extension
  // is running in.
  if (allowed && api->IsPrivileged(params.name))
    allowed = process_map.Contains(extension->id(), requesting_process_id);

  if (!allowed) {
    LOG(ERROR) << "Extension API call disallowed - name:" << params.name
               << " pid:" << requesting_process_id
                << " from URL " << params.source_url.spec();
     SendAccessDenied(ipc_sender, routing_id, params.request_id);
     return NULL;
  }

  ExtensionFunction* function =
      ExtensionFunctionRegistry::GetInstance()->NewFunction(params.name);
  function->SetArgs(&params.arguments);
  function->set_source_url(params.source_url);
  function->set_request_id(params.request_id);
  function->set_has_callback(params.has_callback);
  function->set_user_gesture(params.user_gesture);
  function->set_extension(extension);
  function->set_profile_id(profile);

  UIThreadExtensionFunction* function_ui =
      function->AsUIThreadExtensionFunction();
  if (function_ui) {
    function_ui->SetRenderViewHost(render_view_host);
  }

  return function;
}
