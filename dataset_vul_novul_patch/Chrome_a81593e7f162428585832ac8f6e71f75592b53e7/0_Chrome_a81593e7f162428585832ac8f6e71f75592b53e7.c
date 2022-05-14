void LinkChangeSerializerMarkupAccumulator::appendElement(StringBuilder& result, Element& element, Namespaces* namespaces)
{
    if (element.hasTagName(HTMLNames::htmlTag)) {
         result.append('\n');
        MarkupFormatter::appendComment(result, PageSerializer::markOfTheWebDeclaration(document().url()));
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
