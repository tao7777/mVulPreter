void SerializerMarkupAccumulator::appendElement(StringBuilder& result, Element* element, Namespaces* namespaces)
 {
     if (!shouldIgnoreElement(element))
        MarkupAccumulator::appendElement(result, element, namespaces);
 
        result.appendLiteral("<meta charset=\"");
        result.append(m_document->charset());
        if (m_document->isXHTMLDocument())
            result.appendLiteral("\" />");
        else
            result.appendLiteral("\">");
     }
 
 }
