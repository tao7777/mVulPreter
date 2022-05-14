void LinkChangeSerializerMarkupAccumulator::appendElement(StringBuilder& result, Element& element, Namespaces* namespaces)
{
    if (element.hasTagName(HTMLNames::htmlTag)) {
         result.append('\n');
        MarkupFormatter::appendComment(result, String::format(" saved from url=(%04d)%s ",
            static_cast<int>(document().url().string().utf8().length()),
            document().url().string().utf8().data()));
         result.append('\n');
     }
 
    if (element.hasTagName(HTMLNames::baseTag)) {
        result.appendLiteral("<base href=\".\"");
        if (!document().baseTarget().isEmpty()) {
            result.appendLiteral(" target=\"");
            MarkupFormatter::appendAttributeValue(result, document().baseTarget(), document().isHTMLDocument());
            result.append('"');
        }
        if (document().isXHTMLDocument())
            result.appendLiteral(" />");
        else
            result.appendLiteral(">");
    } else {
        SerializerMarkupAccumulator::appendElement(result, element, namespaces);
    }
}
