bool CookiesFunction::ParseCookieStore(const DictionaryValue* details,
                                       net::CookieStore** store,
                                       std::string* store_id) {
   DCHECK(details && (store || store_id));
   Profile* store_profile = NULL;
   if (details->HasKey(keys::kStoreIdKey)) {
    // The store ID was explicitly specified in the details dictionary.
    // Retrieve its corresponding cookie store.
     std::string store_id_value;
    // Get the store ID string or return false.
     EXTENSION_FUNCTION_VALIDATE(
         details->GetString(keys::kStoreIdKey, &store_id_value));
    store_profile = extension_cookies_helpers::ChooseProfileFromStoreId(
         store_id_value, profile(), include_incognito());
     if (!store_profile) {
       error_ = ExtensionErrorUtils::FormatErrorMessage(
           keys::kInvalidStoreIdError, store_id_value);
       return false;
     }
   } else {
    // The store ID was not specified; use the current execution context's
    // cookie store by default.
     Browser* current_browser = GetCurrentBrowser();
     if (!current_browser) {
      error_ = keys::kNoCookieStoreFoundError;
      return false;
    }
    store_profile = current_browser->profile();
  }
  DCHECK(store_profile);
   if (store)
     *store = store_profile->GetRequestContext()->GetCookieStore();
   if (store_id)
    *store_id =
        extension_cookies_helpers::GetStoreIdFromProfile(store_profile);
   return true;
 }
