void SerializerMarkupAccumulator::appendElement(StringBuilder& result, Element* element, Namespaces* namespaces)
void SerializerMarkupAccumulator::appendElement(StringBuilder& out, Element* element, Namespaces* namespaces)
 {
     if (!shouldIgnoreElement(element))
        MarkupAccumulator::appendElement(out, element, namespaces);
 
        out.append("<meta charset=\"");
        out.append(m_document->charset());
        out.append("\">");
     }
 
 }
