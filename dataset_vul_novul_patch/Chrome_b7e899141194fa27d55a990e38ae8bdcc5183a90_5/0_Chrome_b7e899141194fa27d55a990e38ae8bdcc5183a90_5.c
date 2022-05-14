bool GetAllCookieStoresFunction::RunImpl() {
  Profile* original_profile = profile()->GetOriginalProfile();
  DCHECK(original_profile);
  scoped_ptr<ListValue> original_tab_ids(new ListValue());
  Profile* incognito_profile = NULL;
  scoped_ptr<ListValue> incognito_tab_ids;
  if (include_incognito()) {
    incognito_profile = profile()->GetOffTheRecordProfile();
     if (incognito_profile)
       incognito_tab_ids.reset(new ListValue());
   }
  // Iterate through all browser instances, and for each browser,
  // add its tab IDs to either the regular or incognito tab ID list depending
  // whether the browser is regular or incognito.
   for (BrowserList::const_iterator iter = BrowserList::begin();
        iter != BrowserList::end(); ++iter) {
     Browser* browser = *iter;
     if (browser->profile() == original_profile) {
      extension_cookies_helpers::AppendToTabIdList(browser,
                                                   original_tab_ids.get());
     } else if (incognito_tab_ids.get() &&
                browser->profile() == incognito_profile) {
      extension_cookies_helpers::AppendToTabIdList(browser,
                                                   incognito_tab_ids.get());
     }
   }
  // Return a list of all cookie stores with at least one open tab.
   ListValue* cookie_store_list = new ListValue();
   if (original_tab_ids->GetSize() > 0) {
    cookie_store_list->Append(
        extension_cookies_helpers::CreateCookieStoreValue(
            original_profile, original_tab_ids.release()));
   }
   if (incognito_tab_ids.get() && incognito_tab_ids->GetSize() > 0) {
    cookie_store_list->Append(
        extension_cookies_helpers::CreateCookieStoreValue(
            incognito_profile, incognito_tab_ids.release()));
   }
   result_.reset(cookie_store_list);
   return true;
}
