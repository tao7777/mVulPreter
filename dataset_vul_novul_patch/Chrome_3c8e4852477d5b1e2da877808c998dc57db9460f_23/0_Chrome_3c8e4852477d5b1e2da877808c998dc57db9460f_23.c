void PageHandler::SetRenderer(RenderProcessHost* process_host,
void PageHandler::SetRenderer(int process_host_id,
                               RenderFrameHostImpl* frame_host) {
   if (host_ == frame_host)
     return;

  RenderWidgetHostImpl* widget_host =
      host_ ? host_->GetRenderWidgetHost() : nullptr;
  if (widget_host) {
    registrar_.Remove(
        this,
        content::NOTIFICATION_RENDER_WIDGET_VISIBILITY_CHANGED,
        content::Source<RenderWidgetHost>(widget_host));
  }

  host_ = frame_host;
  widget_host = host_ ? host_->GetRenderWidgetHost() : nullptr;

  if (widget_host) {
    registrar_.Add(
        this,
        content::NOTIFICATION_RENDER_WIDGET_VISIBILITY_CHANGED,
        content::Source<RenderWidgetHost>(widget_host));
  }
}
