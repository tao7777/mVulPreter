bool SniffMimeType(const char* content,
                   size_t content_size,
                   const GURL& url,
                   const std::string& type_hint,
                   std::string* result) {
  DCHECK_LT(content_size, 1000000U);  // sanity check
  DCHECK(content);
  DCHECK(result);

  bool have_enough_content = true;

  result->assign(type_hint);

  if (IsOfficeType(type_hint))
    return SniffForInvalidOfficeDocs(content, content_size, url, result);

   const bool hint_is_unknown_mime_type = IsUnknownMimeType(type_hint);
 
  // First check for HTML
  if (hint_is_unknown_mime_type) {
    // We're only willing to sniff HTML if the server has not supplied a mime
    // type, or if the type it did supply indicates that it doesn't know what
    // the type should be.
    if (SniffForHTML(content, content_size, &have_enough_content, result))
      return true;  // We succeeded in sniffing HTML.  No more content needed.
   }
 
  const bool hint_is_text_plain = (type_hint == "text/plain");
  if (hint_is_unknown_mime_type || hint_is_text_plain) {
    if (!SniffBinary(content, content_size, &have_enough_content, result)) {
      if (hint_is_text_plain) {
        return have_enough_content;
      }
    }
  }

  if (type_hint == "text/xml" || type_hint == "application/xml") {
    if (SniffXML(content, content_size, &have_enough_content, result))
      return true;
    return have_enough_content;
  }

  if (SniffCRX(content, content_size, url, type_hint,
               &have_enough_content, result))
    return true;

  if (SniffForOfficeDocs(content, content_size, url,
                         &have_enough_content, result))
    return true;  // We've matched a magic number.  No more content needed.

  if (type_hint == "application/octet-stream")
    return have_enough_content;

  if (SniffForMagicNumbers(content, content_size,
                           &have_enough_content, result))
    return true;  // We've matched a magic number.  No more content needed.

  return have_enough_content;
}
