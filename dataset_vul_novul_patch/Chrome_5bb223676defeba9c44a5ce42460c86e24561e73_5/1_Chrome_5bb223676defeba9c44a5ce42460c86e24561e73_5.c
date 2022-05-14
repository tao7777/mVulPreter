ExtensionsGuestViewMessageFilter::FrameNavigationHelper::FrameNavigationHelper(
    RenderFrameHost* plugin_rfh,
    int32_t guest_instance_id,
    int32_t element_instance_id,
    bool is_full_page_plugin,
    ExtensionsGuestViewMessageFilter* filter)
    : content::WebContentsObserver(
          content::WebContents::FromRenderFrameHost(plugin_rfh)),
      frame_tree_node_id_(plugin_rfh->GetFrameTreeNodeId()),
      guest_instance_id_(guest_instance_id),
      element_instance_id_(element_instance_id),
      is_full_page_plugin_(is_full_page_plugin),
      filter_(filter),
      parent_site_instance_(plugin_rfh->GetParent()->GetSiteInstance()),
      weak_factory_(this) {
  DCHECK(GetGuestView());
  NavigateToAboutBlank();
  base::PostDelayedTaskWithTraits(
      FROM_HERE, {BrowserThread::UI},
      base::BindOnce(&ExtensionsGuestViewMessageFilter::FrameNavigationHelper::
                         CancelPendingTask,
                     weak_factory_.GetWeakPtr()),
      base::TimeDelta::FromMilliseconds(kAttachFailureDelayMS));
}
