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
   for (BrowserList::const_iterator iter = BrowserList::begin();
        iter != BrowserList::end(); ++iter) {
     Browser* browser = *iter;
     if (browser->profile() == original_profile) {
      helpers::AppendToTabIdList(browser, original_tab_ids.get());
     } else if (incognito_tab_ids.get() &&
                browser->profile() == incognito_profile) {
      helpers::AppendToTabIdList(browser, incognito_tab_ids.get());
     }
   }
   ListValue* cookie_store_list = new ListValue();
   if (original_tab_ids->GetSize() > 0) {
    cookie_store_list->Append(helpers::CreateCookieStoreValue(
        original_profile, original_tab_ids.release()));
   }
   if (incognito_tab_ids.get() && incognito_tab_ids->GetSize() > 0) {
    cookie_store_list->Append(helpers::CreateCookieStoreValue(
        incognito_profile, incognito_tab_ids.release()));
   }
   result_.reset(cookie_store_list);
   return true;
}
