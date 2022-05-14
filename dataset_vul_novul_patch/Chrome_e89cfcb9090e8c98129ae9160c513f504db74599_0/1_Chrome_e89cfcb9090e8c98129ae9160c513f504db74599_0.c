void BrowserEventRouter::TabDetachedAt(TabContents* contents, int index) {
  if (!GetTabEntry(contents->web_contents())) {
     return;
   }
 
   scoped_ptr<ListValue> args(new ListValue());
  args->Append(Value::CreateIntegerValue(
      ExtensionTabUtil::GetTabId(contents->web_contents())));
 
   DictionaryValue* object_args = new DictionaryValue();
   object_args->Set(tab_keys::kOldWindowIdKey, Value::CreateIntegerValue(
      ExtensionTabUtil::GetWindowIdOfTab(contents->web_contents())));
   object_args->Set(tab_keys::kOldPositionKey, Value::CreateIntegerValue(
       index));
   args->Append(object_args);
 
  DispatchEvent(contents->profile(), events::kOnTabDetached, args.Pass(),
                 EventRouter::USER_GESTURE_UNKNOWN);
 }
