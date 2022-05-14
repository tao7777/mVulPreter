void SVGImage::setContainerSize(const IntSize& size)
{
    if (!m_page || !usesContainerSize())
        return;

    LocalFrame* frame = m_page->mainFrame();
     SVGSVGElement* rootElement = toSVGDocument(frame->document())->rootElement();
     if (!rootElement)
         return;
    RenderSVGRoot* renderer = toRenderSVGRoot(rootElement->renderer());
    if (!renderer)
        return;
 
     FrameView* view = frameView();
     view->resize(this->containerSize());
 
     renderer->setContainerSize(size);
 }
