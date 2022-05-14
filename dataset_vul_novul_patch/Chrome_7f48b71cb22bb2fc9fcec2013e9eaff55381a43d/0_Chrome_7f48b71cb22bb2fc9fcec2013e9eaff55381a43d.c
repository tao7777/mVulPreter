void RenderMessageFilter::OnCreateWindow(
    const ViewHostMsg_CreateWindow_Params& params,
    int* route_id,
    int* surface_id,
    int64* cloned_session_storage_namespace_id) {
   bool no_javascript_access;
   bool can_create_window =
       GetContentClient()->browser()->CanCreateWindow(
          params.opener_url,
          params.opener_security_origin,
           params.window_container_type,
           resource_context_,
           render_process_id_,
          &no_javascript_access);

  if (!can_create_window) {
    *route_id = MSG_ROUTING_NONE;
    *surface_id = 0;
    return;
  }

  scoped_refptr<SessionStorageNamespaceImpl> cloned_namespace =
      new SessionStorageNamespaceImpl(dom_storage_context_,
                                      params.session_storage_namespace_id);
  *cloned_session_storage_namespace_id = cloned_namespace->id();

  render_widget_helper_->CreateNewWindow(params,
                                         no_javascript_access,
                                         peer_handle(),
                                         route_id,
                                         surface_id,
                                         cloned_namespace);
}
