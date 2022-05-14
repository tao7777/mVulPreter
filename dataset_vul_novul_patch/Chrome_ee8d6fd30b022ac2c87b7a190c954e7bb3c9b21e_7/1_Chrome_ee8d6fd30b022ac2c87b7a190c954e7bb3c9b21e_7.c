void RenderWidget::OnMsgPaintAtSize(const TransportDIB::Handle& dib_handle,
                                    const gfx::Size& desired_size) {
  if (!webwidget_ || dib_handle == TransportDIB::DefaultHandleValue())
    return;

  if (webwidget_->size().isEmpty() ||
      desired_size.IsEmpty()) {
    Send(new ViewHostMsg_PaintAtSize_ACK(routing_id_,
                                         dib_handle,
                                         desired_size));
    return;
  }

  scoped_ptr<TransportDIB> paint_at_scale_buffer(TransportDIB::Map(dib_handle));

  DCHECK(paint_at_scale_buffer.get());
  if (!paint_at_scale_buffer.get())
    return;

  webwidget_->layout();

  gfx::Size canvas_size = webwidget_->size();
  float x_scale = static_cast<float>(desired_size.width()) /
                  static_cast<float>(canvas_size.width());
   float y_scale = static_cast<float>(desired_size.height()) /
                   static_cast<float>(canvas_size.height());
 
  gfx::Rect orig_bounds(gfx::Point(0,0), canvas_size);
   canvas_size.set_width(static_cast<int>(canvas_size.width() * x_scale));
   canvas_size.set_height(static_cast<int>(canvas_size.height() * y_scale));
  gfx::Rect bounds(gfx::Point(0,0), canvas_size);
 
   scoped_ptr<skia::PlatformCanvas> canvas(
       paint_at_scale_buffer->GetPlatformCanvas(canvas_size.width(),
                                               canvas_size.height()));
  if (!canvas.get()) {
    NOTREACHED();
    return;
  }

  DCHECK_EQ(bounds.width(), canvas->getDevice()->width());
  DCHECK_EQ(bounds.height(), canvas->getDevice()->height());
  bounds.set_width(canvas->getDevice()->width());
  bounds.set_height(canvas->getDevice()->height());

  canvas->save();
  canvas->scale(SkFloatToScalar(x_scale), SkFloatToScalar(y_scale));

  PaintRect(orig_bounds, orig_bounds.origin(), canvas.get());
  canvas->restore();

  Send(new ViewHostMsg_PaintAtSize_ACK(routing_id_, dib_handle, bounds.size()));
}
