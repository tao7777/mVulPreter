void BrowserEventRouter::TabDetachedAt(TabContents* contents, int index) {
void BrowserEventRouter::TabDetachedAt(WebContents* contents, int index) {
  if (!GetTabEntry(contents)) {
     return;
   }
 
   scoped_ptr<ListValue> args(new ListValue());
  args->Append(Value::CreateIntegerValue(ExtensionTabUtil::GetTabId(contents)));
 
   DictionaryValue* object_args = new DictionaryValue();
   object_args->Set(tab_keys::kOldWindowIdKey, Value::CreateIntegerValue(
      ExtensionTabUtil::GetWindowIdOfTab(contents)));
   object_args->Set(tab_keys::kOldPositionKey, Value::CreateIntegerValue(
       index));
   args->Append(object_args);
 
  Profile* profile = Profile::FromBrowserContext(contents->GetBrowserContext());
  DispatchEvent(profile, events::kOnTabDetached, args.Pass(),
                 EventRouter::USER_GESTURE_UNKNOWN);
 }
