void PageSerializer::serializeFrame(Frame* frame)
{
    Document* document = frame->document();
    KURL url = document->url();
    if (!url.isValid() || url.isBlankURL()) {
        url = urlForBlankFrame(frame);
    }

    if (m_resourceURLs.contains(url)) {
         return;
     }
 
     Vector<Node*> nodes;
    SerializerMarkupAccumulator accumulator(this, document, &nodes);
     WTF::TextEncoding textEncoding(document->charset());
    CString data;
    if (!textEncoding.isValid()) {
        // FIXME: iframes used as images trigger this. We should deal with them correctly.
        return;
    }
    String text = accumulator.serializeNodes(document, IncludeNode);
     CString frameHTML = textEncoding.normalizeAndEncode(text, WTF::EntitiesForUnencodables);
     m_resources->append(SerializedResource(url, document->suggestedMIMEType(), SharedBuffer::create(frameHTML.data(), frameHTML.length())));
     m_resourceURLs.add(url);

    for (Vector<Node*>::iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
        Node* node = *iter;
        if (!node->isElementNode())
            continue;
 
         Element* element = toElement(node);
        if (element->isStyledElement())
             retrieveResourcesForProperties(element->inlineStyle(), document);
 
         if (element->hasTagName(HTMLNames::imgTag)) {
             HTMLImageElement* imageElement = toHTMLImageElement(element);
            KURL url = document->completeURL(imageElement->getAttribute(HTMLNames::srcAttr));
            ImageResource* cachedImage = imageElement->cachedImage();
            addImageToResources(cachedImage, imageElement->renderer(), url);
        } else if (element->hasTagName(HTMLNames::inputTag)) {
            HTMLInputElement* inputElement = toHTMLInputElement(element);
            if (inputElement->isImageButton() && inputElement->hasImageLoader()) {
                KURL url = inputElement->src();
                ImageResource* cachedImage = inputElement->imageLoader()->image();
                addImageToResources(cachedImage, inputElement->renderer(), url);
            }
        } else if (element->hasTagName(HTMLNames::linkTag)) {
            HTMLLinkElement* linkElement = toHTMLLinkElement(element);
            if (CSSStyleSheet* sheet = linkElement->sheet()) {
                KURL url = document->completeURL(linkElement->getAttribute(HTMLNames::hrefAttr));
                serializeCSSStyleSheet(sheet, url);
                ASSERT(m_resourceURLs.contains(url));
            }
        } else if (element->hasTagName(HTMLNames::styleTag)) {
            HTMLStyleElement* styleElement = toHTMLStyleElement(element);
            if (CSSStyleSheet* sheet = styleElement->sheet())
                serializeCSSStyleSheet(sheet, KURL());
        }
    }

    for (Frame* childFrame = frame->tree().firstChild(); childFrame; childFrame = childFrame->tree().nextSibling())
        serializeFrame(childFrame);
}
