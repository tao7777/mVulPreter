void SVGImage::setContainerSize(const IntSize& size)
{
    if (!m_page || !usesContainerSize())
        return;

    LocalFrame* frame = m_page->mainFrame();
     SVGSVGElement* rootElement = toSVGDocument(frame->document())->rootElement();
     if (!rootElement)
         return;
 
     FrameView* view = frameView();
     view->resize(this->containerSize());
 
    RenderSVGRoot* renderer = toRenderSVGRoot(rootElement->renderer());
    if (!renderer)
        return;
     renderer->setContainerSize(size);
 }
