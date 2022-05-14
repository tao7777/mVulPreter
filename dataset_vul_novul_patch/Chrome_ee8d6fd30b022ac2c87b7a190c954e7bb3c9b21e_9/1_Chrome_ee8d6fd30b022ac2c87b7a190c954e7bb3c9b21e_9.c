void RenderWidget::didScrollRect(int dx, int dy, const WebRect& clip_rect) {
   bool update_pending = paint_aggregator_.HasPendingUpdate();
 
  gfx::Rect view_rect(0, 0, size_.width(), size_.height());
   gfx::Rect damaged_rect = view_rect.Intersect(clip_rect);
   if (damaged_rect.IsEmpty())
     return;

  paint_aggregator_.ScrollRect(dx, dy, damaged_rect);

  if (update_pending)
    return;
  if (!paint_aggregator_.HasPendingUpdate())
    return;
  if (update_reply_pending())
    return;

  MessageLoop::current()->PostTask(FROM_HERE, NewRunnableMethod(
      this, &RenderWidget::CallDoDeferredUpdate));
}
