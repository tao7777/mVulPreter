 bool RemoveCookieFunction::RunImpl() {
   DictionaryValue* details;
   EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &details));
 
   GURL url;
  if (!ParseUrl(details, &url))
    return false;
  if (!GetExtension()->HasHostPermission(url)) {
    error_ = ExtensionErrorUtils::FormatErrorMessage(
        keys::kNoHostPermissionsError, url.spec());
     return false;
  }
 
   std::string name;
   EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kNameKey, &name));
 
   net::CookieStore* cookie_store;
  if (!ParseCookieStore(details, &cookie_store, NULL))
    return false;
  DCHECK(cookie_store);

  cookie_store->DeleteCookie(url, name);
  return true;
}
