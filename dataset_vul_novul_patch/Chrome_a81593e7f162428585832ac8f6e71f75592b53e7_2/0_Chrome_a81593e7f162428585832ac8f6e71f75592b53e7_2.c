 WebString WebPageSerializer::generateMetaCharsetDeclaration(const WebString& charset)
 {
    // TODO(yosin) We should call |PageSerializer::metaCharsetDeclarationOf()|.
     String charsetString = "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=" + static_cast<const String&>(charset) + "\">";
     return charsetString;
 }
