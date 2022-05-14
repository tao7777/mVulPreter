 void WebPageSerializerImpl::openTagToString(Element* element,
                                             SerializeDomParam* param)
 {
    bool needSkip;
    StringBuilder result;
    result.append(preActionBeforeSerializeOpenTag(element, param, &needSkip));
    if (needSkip)
        return;
    result.append('<');
    result.append(element->nodeName().lower());
    AttributeCollection attributes = element->attributes();
    AttributeCollection::iterator end = attributes.end();
    for (AttributeCollection::iterator it = attributes.begin(); it != end; ++it) {
        result.append(' ');
        result.append(it->name().toString());
        result.appendLiteral("=\"");
        if (!it->value().isEmpty()) {
            const String& attrValue = it->value();

            const QualifiedName& attrName = it->name();
             if (element->hasLegalLinkAttribute(attrName)) {
                 if (attrValue.startsWith("javascript:", TextCaseInsensitive)) {
                    result.append(m_htmlEntities.convertEntitiesInString(attrValue));
                 } else {
                     WebLocalFrameImpl* subFrame = WebLocalFrameImpl::fromFrameOwnerElement(element);
                    String completeURL = subFrame ? subFrame->frame()->document()->url() :
                                                    param->document->completeURL(attrValue);
                    if (m_localLinks.contains(completeURL)) {
                        if (!param->directoryName.isEmpty()) {
                            result.appendLiteral("./");
                             result.append(param->directoryName);
                             result.append('/');
                         }
                        result.append(m_htmlEntities.convertEntitiesInString(m_localLinks.get(completeURL)));
                     } else {
                        result.append(m_htmlEntities.convertEntitiesInString(completeURL));
                     }
                 }
             } else {
                if (param->isHTMLDocument)
                    result.append(m_htmlEntities.convertEntitiesInString(attrValue));
                else
                    result.append(m_xmlEntities.convertEntitiesInString(attrValue));
            }
        }
        result.append('\"');
    }

    String addedContents = postActionAfterSerializeOpenTag(element, param);
    if (element->hasChildren() || param->haveAddedContentsBeforeEnd)
        result.append('>');
    result.append(addedContents);
    saveHTMLContentToBuffer(result.toString(), param);
}
