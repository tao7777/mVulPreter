 WebString WebPageSerializer::generateBaseTagDeclaration(const WebString& baseTarget)
 {
    // TODO(yosin) We should call |PageSerializer::baseTagDeclarationOf()|.
     if (baseTarget.isEmpty())
         return String("<base href=\".\">");
     String baseString = "<base href=\".\" target=\"" + static_cast<const String&>(baseTarget) + "\">";
    return baseString;
}
