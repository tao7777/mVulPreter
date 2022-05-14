bool CookiesFunction::ParseCookieStore(const DictionaryValue* details,
                                       net::CookieStore** store,
                                       std::string* store_id) {
   DCHECK(details && (store || store_id));
   Profile* store_profile = NULL;
   if (details->HasKey(keys::kStoreIdKey)) {
     std::string store_id_value;
     EXTENSION_FUNCTION_VALIDATE(
         details->GetString(keys::kStoreIdKey, &store_id_value));
    store_profile = helpers::ChooseProfileFromStoreId(
         store_id_value, profile(), include_incognito());
     if (!store_profile) {
       error_ = ExtensionErrorUtils::FormatErrorMessage(
           keys::kInvalidStoreIdError, store_id_value);
       return false;
     }
   } else {
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
    *store_id = helpers::GetStoreIdFromProfile(store_profile);
   return true;
 }
