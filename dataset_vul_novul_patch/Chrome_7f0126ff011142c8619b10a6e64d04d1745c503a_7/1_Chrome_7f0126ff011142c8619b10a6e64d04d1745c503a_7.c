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
 
    if (document->isImageDocument()) {
        ImageDocument* imageDocument = toImageDocument(document);
        addImageToResources(imageDocument->cachedImage(), imageDocument->imageElement()->renderer(), url);
        return;
    }
     Vector<Node*> nodes;
    OwnPtr<SerializerMarkupAccumulator> accumulator;
    if (m_URLs)
        accumulator = adoptPtr(new LinkChangeSerializerMarkupAccumulator(this, document, &nodes, m_URLs, m_directory));
    else
        accumulator = adoptPtr(new SerializerMarkupAccumulator(this, document, &nodes));
    String text = accumulator->serializeNodes(document, IncludeNode);
     WTF::TextEncoding textEncoding(document->charset());
     CString frameHTML = textEncoding.normalizeAndEncode(text, WTF::EntitiesForUnencodables);
     m_resources->append(SerializedResource(url, document->suggestedMIMEType(), SharedBuffer::create(frameHTML.data(), frameHTML.length())));
     m_resourceURLs.add(url);

    for (Vector<Node*>::iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
        Node* node = *iter;
        if (!node->isElementNode())
            continue;
 
         Element* element = toElement(node);
        if (element->isStyledElement()) {
             retrieveResourcesForProperties(element->inlineStyle(), document);
            retrieveResourcesForProperties(element->presentationAttributeStyle(), document);
        }
 
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
