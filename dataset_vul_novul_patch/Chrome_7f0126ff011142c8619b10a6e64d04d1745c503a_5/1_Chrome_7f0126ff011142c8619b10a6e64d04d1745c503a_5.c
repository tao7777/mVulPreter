void LinkChangeSerializerMarkupAccumulator::appendElement(StringBuilder& result, Element* element, Namespaces* namespaces)
{
    if (element->hasTagName(HTMLNames::baseTag)) {
        result.append("<!--");
    } else if (element->hasTagName(HTMLNames::htmlTag)) {
        result.append(String::format("\n<!-- saved from url=(%04d)%s -->\n",
            static_cast<int>(m_document->url().string().utf8().length()),
            m_document->url().string().utf8().data()));
    }
    SerializerMarkupAccumulator::appendElement(result, element, namespaces);
    if (element->hasTagName(HTMLNames::baseTag)) {
        result.appendLiteral("-->");
        result.appendLiteral("<base href=\".\"");
        if (!m_document->baseTarget().isEmpty()) {
            result.appendLiteral(" target=\"");
            result.append(m_document->baseTarget());
            result.append('"');
        }
        if (m_document->isXHTMLDocument())
            result.appendLiteral(" />");
        else
            result.appendLiteral(">");
    }
}
