 bool GetAllCookiesFunction::RunImpl() {
   DictionaryValue* details;
   EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &details));
 
   GURL url;
  if (details->HasKey(keys::kUrlKey) && !ParseUrl(details, &url))
     return false;
 
   net::CookieStore* cookie_store;
  std::string store_id;
  if (!ParseCookieStore(details, &cookie_store, &store_id))
    return false;
   DCHECK(cookie_store);
 
   ListValue* matching_list = new ListValue();
  helpers::AppendMatchingCookiesToList(cookie_store, store_id, url, details,
                                       GetExtension(), matching_list);
   result_.reset(matching_list);
   return true;
 }
