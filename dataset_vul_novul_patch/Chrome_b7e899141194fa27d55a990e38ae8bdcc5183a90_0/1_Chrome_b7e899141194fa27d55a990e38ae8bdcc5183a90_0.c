bool CookiesFunction::ParseUrl(const DictionaryValue* details, GURL* url) {
   DCHECK(details && url);
   std::string url_string;
   EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kUrlKey, &url_string));
   *url = GURL(url_string);
   if (!url->is_valid()) {
     error_ = ExtensionErrorUtils::FormatErrorMessage(
         keys::kInvalidUrlError, url_string);
     return false;
   }
   return true;
 }
