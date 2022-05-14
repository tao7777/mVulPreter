void RenderWidgetHostImpl::WasShown(const ui::LatencyInfo& latency_info) {
  if (!is_hidden_)
    return;

  TRACE_EVENT0("renderer_host", "RenderWidgetHostImpl::WasShown");
  is_hidden_ = false;
 
  ForceFirstFrameAfterNavigationTimeout();
 
   SendScreenRects();
   RestartHangMonitorTimeoutIfNecessary();

  bool needs_repainting = true;
  needs_repainting_on_restore_ = false;
  Send(new ViewMsg_WasShown(routing_id_, needs_repainting, latency_info));

  process_->WidgetRestored();

  bool is_visible = true;
  NotificationService::current()->Notify(
      NOTIFICATION_RENDER_WIDGET_VISIBILITY_CHANGED,
      Source<RenderWidgetHost>(this),
      Details<bool>(&is_visible));

  WasResized();
}
