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
 
  if (api->IsPrivileged(params.name) &&
      !process_map.Contains(extension->id(), requesting_process_id)) {
    LOG(ERROR) << "Extension API called from incorrect process "
               << requesting_process_id
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
