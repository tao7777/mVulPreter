bool WebPluginDelegateProxy::BindFakePluginWindowHandle(bool opaque) {
  gfx::PluginWindowHandle fake_window = NULL;
  if (render_view_)
    fake_window = render_view_->AllocateFakePluginWindowHandle(opaque);
  if (!fake_window)
    return false;
  OnSetWindow(fake_window);
  if (!Send(new PluginMsg_SetFakeAcceleratedSurfaceWindowHandle(instance_id_,
                                                                fake_window))) {
    return false;
  }

   webkit_glue::WebPluginGeometry geom;
   geom.window = fake_window;
   geom.window_rect = plugin_rect_;
  geom.clip_rect = gfx::Rect(plugin_rect_.size());
   geom.rects_valid = true;
   geom.visible = true;
   render_view_->DidMovePlugin(geom);
  render_view_->didInvalidateRect(WebKit::WebRect(plugin_rect_.x(),
                                                  plugin_rect_.y(),
                                                  plugin_rect_.width(),
                                                  plugin_rect_.height()));

  return true;
}
