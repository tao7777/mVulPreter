WebKit::WebFrame* RenderViewImpl::GetFrameByMappedID(int frame_id) {
WebKit::WebFrame* RenderViewImpl::GetFrameByRemoteID(int remote_frame_id) {
  std::map<int, int>::const_iterator it = active_frame_id_map_.begin();
  for (; it != active_frame_id_map_.end(); ++it) {
    if (it->second == remote_frame_id)
      return FindFrameByID(webview()->mainFrame(), it->first);
  }
  return NULL;
 }
