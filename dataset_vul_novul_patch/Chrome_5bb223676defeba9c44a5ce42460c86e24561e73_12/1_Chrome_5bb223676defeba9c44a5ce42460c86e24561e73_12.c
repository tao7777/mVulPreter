void ExtensionsGuestViewMessageFilter::ResumeAttachOrDestroy(
    int32_t element_instance_id,
    int32_t plugin_frame_routing_id) {
  auto it = frame_navigation_helpers_.find(element_instance_id);
  if (it == frame_navigation_helpers_.end()) {
    return;
  }
  auto* plugin_rfh = content::RenderFrameHost::FromID(render_process_id_,
                                                      plugin_frame_routing_id);
  auto* helper = it->second.get();
  auto* guest_view = helper->GetGuestView();
  if (!guest_view)
    return;
 
  if (plugin_rfh) {
    DCHECK(
        guest_view->web_contents()->CanAttachToOuterContentsFrame(plugin_rfh));
    guest_view->AttachToOuterWebContentsFrame(plugin_rfh, element_instance_id,
                                              helper->is_full_page_plugin());
  } else {
    guest_view->GetEmbedderFrame()->Send(
        new ExtensionsGuestViewMsg_DestroyFrameContainer(element_instance_id));
    guest_view->Destroy(true);
  }
  frame_navigation_helpers_.erase(element_instance_id);
 }
