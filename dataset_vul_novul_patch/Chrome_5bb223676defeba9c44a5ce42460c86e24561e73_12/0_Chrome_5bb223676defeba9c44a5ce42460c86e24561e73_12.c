void ExtensionsGuestViewMessageFilter::ResumeAttachOrDestroy(
 
  MimeHandlerViewAttachHelper::Get(render_process_id_)
      ->AttachToOuterWebContents(guest_view, embedder_render_process_id,
                                 plugin_frame_routing_id, element_instance_id,
                                 is_full_page_plugin);
 }
