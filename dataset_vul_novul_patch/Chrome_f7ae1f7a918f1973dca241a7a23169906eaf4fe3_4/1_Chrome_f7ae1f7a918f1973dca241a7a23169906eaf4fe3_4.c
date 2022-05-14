void BrowserEventRouter::TabPinnedStateChanged(WebContents* contents,
                                               int index) {
  TabStripModel* tab_strip = NULL;
   int tab_index;
 
   if (ExtensionTabUtil::GetTabStripModel(contents, &tab_strip, &tab_index)) {
    DictionaryValue* changed_properties = new DictionaryValue();
     changed_properties->SetBoolean(tab_keys::kPinnedKey,
                                    tab_strip->IsTabPinned(tab_index));
    DispatchTabUpdatedEvent(contents, changed_properties);
   }
 }
