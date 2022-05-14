WebKit::WebFrame* RenderViewImpl::GetFrameByMappedID(int frame_id) {
  std::map<int, int>::iterator it = active_frame_id_map_.find(frame_id);
  if (it == active_frame_id_map_.end())
    return NULL;
  return FindFrameByID(webview()->mainFrame(), it->second);
 }
