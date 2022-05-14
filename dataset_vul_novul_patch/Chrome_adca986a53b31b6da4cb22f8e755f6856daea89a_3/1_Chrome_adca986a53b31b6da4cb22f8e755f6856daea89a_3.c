RenderFrameHostManager::RenderFrameHostManager(
    FrameTreeNode* frame_tree_node,
    RenderFrameHostDelegate* render_frame_delegate,
    RenderWidgetHostDelegate* render_widget_delegate,
    Delegate* delegate)
    : frame_tree_node_(frame_tree_node),
       delegate_(delegate),
       render_frame_delegate_(render_frame_delegate),
       render_widget_delegate_(render_widget_delegate),
      interstitial_page_(nullptr),
       weak_factory_(this) {
   DCHECK(frame_tree_node_);
 }
