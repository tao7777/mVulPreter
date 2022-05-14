 FrameImpl::~FrameImpl() {
  if (window_tree_host_) {
    window_tree_host_->Hide();
    window_tree_host_->compositor()->SetVisible(false);
  }
}

zx::unowned_channel FrameImpl::GetBindingChannelForTest() const {
  return zx::unowned_channel(binding_.channel());
 }
