void BrowserEventRouter::ExtensionActionExecuted(
    Profile* profile,
    const ExtensionAction& extension_action,
    WebContents* web_contents) {
  const char* event_name = NULL;
  switch (extension_action.action_type()) {
    case Extension::ActionInfo::TYPE_BROWSER:
      event_name = "browserAction.onClicked";
      break;
    case Extension::ActionInfo::TYPE_PAGE:
      event_name = "pageAction.onClicked";
      break;
    case Extension::ActionInfo::TYPE_SCRIPT_BADGE:
      event_name = "scriptBadge.onClicked";
      break;
    case Extension::ActionInfo::TYPE_SYSTEM_INDICATOR:
      break;
  }

   if (event_name) {
     scoped_ptr<ListValue> args(new ListValue());
     DictionaryValue* tab_value = ExtensionTabUtil::CreateTabValue(
        web_contents,
        ExtensionTabUtil::INCLUDE_PRIVACY_SENSITIVE_FIELDS);
     args->Append(tab_value);
 
     DispatchEventToExtension(profile,
                             extension_action.extension_id(),
                             event_name,
                             args.Pass(),
                             EventRouter::USER_GESTURE_ENABLED);
  }
}
