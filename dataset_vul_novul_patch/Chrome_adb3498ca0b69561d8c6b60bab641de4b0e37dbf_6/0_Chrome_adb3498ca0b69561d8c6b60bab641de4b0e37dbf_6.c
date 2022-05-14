void GraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    if (paintingDisabled())
         return;
     
     m_data->context->SetPen(wxPen(strokeColor(), strokeThickness(), strokeStyleToWxPenStyle(strokeStyle())));
    m_data->context->DrawEllipticArc(rect.x(), rect.y(), rect.width(), rect.height(), startAngle, startAngle + angleSpan);
 }
