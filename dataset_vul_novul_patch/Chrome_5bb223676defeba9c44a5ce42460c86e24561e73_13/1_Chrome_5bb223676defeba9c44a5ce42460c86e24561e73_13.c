    ShouldCancelAndIgnore(NavigationHandle* handle) {
  return handle->GetFrameTreeNodeId() == frame_tree_node_id_;
}
