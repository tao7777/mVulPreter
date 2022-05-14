void WebPluginDelegateProxy::OnInvalidateRect(const gfx::Rect& rect) {
  if (!plugin_)
    return;
 
  const gfx::Rect clipped_rect(rect.Intersect(
      gfx::Rect(0, 0, plugin_rect_.width(), plugin_rect_.height())));
 
   invalidate_pending_ = true;
   CopyFromTransportToBacking(clipped_rect);
  plugin_->InvalidateRect(clipped_rect);
}
