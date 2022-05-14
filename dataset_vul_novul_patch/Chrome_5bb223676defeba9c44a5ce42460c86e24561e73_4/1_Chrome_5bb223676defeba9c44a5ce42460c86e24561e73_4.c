void ExtensionsGuestViewMessageFilter::FrameNavigationHelper::FrameDeleted(
    RenderFrameHost* render_frame_host) {
  if (render_frame_host->GetFrameTreeNodeId() != frame_tree_node_id_)
    return;
  filter_->ResumeAttachOrDestroy(element_instance_id_,
                                 MSG_ROUTING_NONE /* no plugin frame */);
}
