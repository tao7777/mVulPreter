ResourceRequestInfoImpl* ResourceDispatcherHostImpl::CreateRequestInfo(
     int child_id,
     int render_view_route_id,
     int render_frame_route_id,
    int frame_tree_node_id,
     PreviewsState previews_state,
     bool download,
     ResourceContext* context) {
   return new ResourceRequestInfoImpl(
       ResourceRequesterInfo::CreateForDownloadOrPageSave(child_id),
      render_view_route_id, frame_tree_node_id,
       ChildProcessHost::kInvalidUniqueID,  // plugin_child_id
       MakeRequestID(), render_frame_route_id,
       false,  // is_main_frame
      {},     // fetch_window_id
      RESOURCE_TYPE_SUB_RESOURCE, ui::PAGE_TRANSITION_LINK,
      download,  // is_download
      false,     // is_stream
      download,  // allow_download
      false,     // has_user_gesture
      false,     // enable_load_timing
      false,     // enable_upload_progress
      false,     // do_not_prompt_for_login
      false,     // keepalive
      network::mojom::ReferrerPolicy::kDefault,
      false,  // is_prerendering
      context,
      false,           // report_raw_headers
      false,           // report_security_info
      true,            // is_async
      previews_state,  // previews_state
      nullptr,         // body
      false);          // initiated_in_secure_context
}
