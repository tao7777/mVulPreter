bool CookiesFunction::ParseUrl(const DictionaryValue* details, GURL* url) {
bool CookiesFunction::ParseUrl(const DictionaryValue* details, GURL* url,
                               bool check_host_permissions) {
   DCHECK(details && url);
   std::string url_string;
  // Get the URL string or return false.
   EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kUrlKey, &url_string));
   *url = GURL(url_string);
   if (!url->is_valid()) {
     error_ = ExtensionErrorUtils::FormatErrorMessage(
         keys::kInvalidUrlError, url_string);
     return false;
   }
  // Check against host permissions if needed.
  if (check_host_permissions &&
      !GetExtension()->HasHostPermission(*url)) {
    error_ = ExtensionErrorUtils::FormatErrorMessage(
        keys::kNoHostPermissionsError, url->spec());
    return false;
  }
   return true;
 }
