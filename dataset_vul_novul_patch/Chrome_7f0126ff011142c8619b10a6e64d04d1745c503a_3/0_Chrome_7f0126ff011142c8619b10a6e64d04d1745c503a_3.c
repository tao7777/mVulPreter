void SerializerMarkupAccumulator::appendCustomAttributes(StringBuilder& result, Element* element, Namespaces* namespaces)
void SerializerMarkupAccumulator::appendCustomAttributes(StringBuilder& out, Element* element, Namespaces* namespaces)
 {
     if (!element->isFrameOwnerElement())
         return;

    HTMLFrameOwnerElement* frameOwner = toHTMLFrameOwnerElement(element);
    Frame* frame = frameOwner->contentFrame();
    if (!frame)
        return;

    KURL url = frame->document()->url();
    if (url.isValid() && !url.isBlankURL())
        return;
 
     url = m_serializer->urlForBlankFrame(frame);
    appendAttribute(out, element, Attribute(frameOwnerURLAttributeName(*frameOwner), url.string()), namespaces);
 }
