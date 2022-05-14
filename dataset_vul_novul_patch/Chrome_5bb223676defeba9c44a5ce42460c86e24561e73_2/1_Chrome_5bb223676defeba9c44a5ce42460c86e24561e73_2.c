    DidFinishNavigation(NavigationHandle* handle) {
  if (handle->GetFrameTreeNodeId() != frame_tree_node_id_)
    return;
  if (!handle->HasCommitted())
    return;
  if (handle->GetRenderFrameHost()->GetSiteInstance() != parent_site_instance_)
    return;
  if (!handle->GetURL().IsAboutBlank())
    return;
  if (!handle->GetRenderFrameHost()->PrepareForInnerWebContentsAttach()) {
    filter_->ResumeAttachOrDestroy(element_instance_id_,
                                   MSG_ROUTING_NONE /* no plugin frame */);
  }
  base::PostTaskWithTraits(
      FROM_HERE, {BrowserThread::UI},
      base::BindOnce(&ExtensionsGuestViewMessageFilter::ResumeAttachOrDestroy,
                     filter_, element_instance_id_,
                     handle->GetRenderFrameHost()->GetRoutingID()));
}
