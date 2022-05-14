 WebString WebPageSerializer::generateMetaCharsetDeclaration(const WebString& charset)
 {
     String charsetString = "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=" + static_cast<const String&>(charset) + "\">";
     return charsetString;
 }
