 FrameImpl::~FrameImpl() {
  window_tree_host_->Hide();
  window_tree_host_->compositor()->SetVisible(false);
 }
