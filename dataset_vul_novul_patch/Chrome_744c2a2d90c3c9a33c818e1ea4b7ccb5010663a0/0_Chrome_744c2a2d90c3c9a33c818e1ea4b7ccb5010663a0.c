bool DebuggerFunction::InitTabContents() {
  Value* debuggee;
  EXTENSION_FUNCTION_VALIDATE(args_->Get(0, &debuggee));

  DictionaryValue* dict = static_cast<DictionaryValue*>(debuggee);
  EXTENSION_FUNCTION_VALIDATE(dict->GetInteger(keys::kTabIdKey, &tab_id_));

  contents_ = NULL;
  TabContentsWrapper* wrapper = NULL;
  bool result = ExtensionTabUtil::GetTabById(
      tab_id_, profile(), include_incognito(), NULL, NULL, &wrapper, NULL);
  if (!result || !wrapper) {
    error_ = ExtensionErrorUtils::FormatErrorMessage(
        keys::kNoTabError,
        base::IntToString(tab_id_));
    return false;
   }
   contents_ = wrapper->web_contents();
 
  if (content::GetContentClient()->HasWebUIScheme(
           contents_->GetURL())) {
     error_ = ExtensionErrorUtils::FormatErrorMessage(
         keys::kAttachToWebUIError,
        contents_->GetURL().scheme());
    return false;
  }

  return true;
}
