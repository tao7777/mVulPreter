 bool RemoveCookieFunction::RunImpl() {
  // Return false if the arguments are malformed.
   DictionaryValue* details;
   EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &details));
  DCHECK(details);
 
   GURL url;
  if (!ParseUrl(details, &url, true))
     return false;
 
   std::string name;
  // Get the cookie name string or return false.
   EXTENSION_FUNCTION_VALIDATE(details->GetString(keys::kNameKey, &name));
 
   net::CookieStore* cookie_store;
  if (!ParseCookieStore(details, &cookie_store, NULL))
    return false;
  DCHECK(cookie_store);

  cookie_store->DeleteCookie(url, name);
  return true;
}
