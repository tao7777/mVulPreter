 WebString WebPageSerializer::generateBaseTagDeclaration(const WebString& baseTarget)
 {
     if (baseTarget.isEmpty())
         return String("<base href=\".\">");
     String baseString = "<base href=\".\" target=\"" + static_cast<const String&>(baseTarget) + "\">";
    return baseString;
}
