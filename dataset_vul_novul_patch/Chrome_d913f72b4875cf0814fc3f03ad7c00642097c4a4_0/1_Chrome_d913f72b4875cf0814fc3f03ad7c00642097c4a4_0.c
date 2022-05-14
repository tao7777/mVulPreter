bool CSSStyleSheetResource::CanUseSheet(const CSSParserContext* parser_context,
                                        MIMETypeCheck mime_type_check) const {
  if (ErrorOccurred())
    return false;

  KURL sheet_url = GetResponse().Url();
  if (sheet_url.IsLocalFile()) {
    if (parser_context) {
      parser_context->Count(WebFeature::kLocalCSSFile);
    }
    String extension;
    int last_dot = sheet_url.LastPathComponent().ReverseFind('.');
    if (last_dot != -1)
      extension = sheet_url.LastPathComponent().Substring(last_dot + 1);
    if (!EqualIgnoringASCIICase(
            MIMETypeRegistry::GetMIMETypeForExtension(extension), "text/css")) {
      if (parser_context) {
         parser_context->CountDeprecation(
             WebFeature::kLocalCSSFileExtensionRejected);
       }
      if (RuntimeEnabledFeatures::RequireCSSExtensionForFileEnabled()) {
        return false;
      }
     }
   }
 
  if (mime_type_check == MIMETypeCheck::kLax)
    return true;
  AtomicString content_type = HttpContentType();
  return content_type.IsEmpty() ||
         DeprecatedEqualIgnoringCase(content_type, "text/css") ||
         DeprecatedEqualIgnoringCase(content_type,
                                     "application/x-unknown-content-type");
}
