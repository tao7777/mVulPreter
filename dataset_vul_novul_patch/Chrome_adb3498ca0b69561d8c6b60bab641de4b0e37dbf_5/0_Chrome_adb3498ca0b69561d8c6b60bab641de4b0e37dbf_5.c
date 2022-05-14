void GraphicsContext::fillRoundedRect(const IntRect& rect, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color& color, ColorSpace colorSpace)
{
     if (paintingDisabled())
         return;
     
#if USE(WXGC)
    Path path;
    path.addRoundedRect(rect, topLeft, topRight, bottomLeft, bottomRight);
    m_data->context->SetBrush(wxBrush(color));
    wxGraphicsContext* gc = m_data->context->GetGraphicsContext();
    gc->FillPath(*path.platformPath());
#endif
 }
