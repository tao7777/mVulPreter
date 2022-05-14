 bool GetAllCookiesFunction::RunImpl() {
  // Return false if the arguments are malformed.
   DictionaryValue* details;
   EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &details));
  DCHECK(details);
 
   GURL url;
  if (details->HasKey(keys::kUrlKey) && !ParseUrl(details, &url, false))
     return false;
 
   net::CookieStore* cookie_store;
  std::string store_id;
  if (!ParseCookieStore(details, &cookie_store, &store_id))
    return false;
   DCHECK(cookie_store);
 
   ListValue* matching_list = new ListValue();
  extension_cookies_helpers::AppendMatchingCookiesToList(
      cookie_store, store_id, url, details, GetExtension(), matching_list);
   result_.reset(matching_list);
   return true;
 }
