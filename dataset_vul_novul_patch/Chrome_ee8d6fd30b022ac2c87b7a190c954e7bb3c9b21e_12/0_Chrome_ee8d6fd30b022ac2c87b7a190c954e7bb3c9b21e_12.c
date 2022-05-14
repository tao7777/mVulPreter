void WebPluginDelegateProxy::OnInvalidateRect(const gfx::Rect& rect) {
  if (!plugin_)
    return;
 
  const gfx::Rect clipped_rect(rect.Intersect(gfx::Rect(plugin_rect_.size())));
 
   invalidate_pending_ = true;
   CopyFromTransportToBacking(clipped_rect);
  plugin_->InvalidateRect(clipped_rect);
}
