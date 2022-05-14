 DictionaryValue* ExtensionTabUtil::CreateTabValue(
     const WebContents* contents,
     TabStripModel* tab_strip,
    int tab_index) {
   if (!tab_strip)
     ExtensionTabUtil::GetTabStripModel(contents, &tab_strip, &tab_index);
 
  DictionaryValue* result = new DictionaryValue();
  bool is_loading = contents->IsLoading();
  result->SetInteger(keys::kIdKey, GetTabId(contents));
  result->SetInteger(keys::kIndexKey, tab_index);
  result->SetInteger(keys::kWindowIdKey, GetWindowIdOfTab(contents));
  result->SetString(keys::kStatusKey, GetTabStatusText(is_loading));
  result->SetBoolean(keys::kActiveKey,
                     tab_strip && tab_index == tab_strip->active_index());
  result->SetBoolean(keys::kSelectedKey,
                     tab_strip && tab_index == tab_strip->active_index());
  result->SetBoolean(keys::kHighlightedKey,
                   tab_strip && tab_strip->IsTabSelected(tab_index));
  result->SetBoolean(keys::kPinnedKey,
                     tab_strip && tab_strip->IsTabPinned(tab_index));
   result->SetBoolean(keys::kIncognitoKey,
                      contents->GetBrowserContext()->IsOffTheRecord());
 
  // Privacy-sensitive fields: these should be stripped off by
  // ScrubTabValueForExtension if the extension should not see them.
  result->SetString(keys::kUrlKey, contents->GetURL().spec());
  result->SetString(keys::kTitleKey, contents->GetTitle());
  if (!is_loading) {
    NavigationEntry* entry = contents->GetController().GetActiveEntry();
    if (entry && entry->GetFavicon().valid)
      result->SetString(keys::kFaviconUrlKey, entry->GetFavicon().url.spec());
   }
 
   if (tab_strip) {
    WebContents* opener = tab_strip->GetOpenerOfWebContentsAt(tab_index);
    if (opener)
      result->SetInteger(keys::kOpenerTabIdKey, GetTabId(opener));
  }

   return result;
 }
