void TestingAutomationProvider::GetExtensionsInfo(
    DictionaryValue* args,
    IPC::Message* reply_message) {
  AutomationJSONReply reply(this, reply_message);
  ExtensionService* service = profile()->GetExtensionService();
  if (!service) {
    reply.SendError("No extensions service.");
    return;
  }
  scoped_ptr<DictionaryValue> return_value(new DictionaryValue);
  ListValue* extensions_values = new ListValue;
  const ExtensionSet* extensions = service->extensions();
  const ExtensionSet* disabled_extensions = service->disabled_extensions();
  ExtensionList all;
  all.insert(all.end(),
             extensions->begin(),
             extensions->end());
  all.insert(all.end(),
             disabled_extensions->begin(),
             disabled_extensions->end());
  for (ExtensionList::const_iterator it = all.begin();
       it != all.end(); ++it) {
    const Extension* extension = *it;
    std::string id = extension->id();
    DictionaryValue* extension_value = new DictionaryValue;
    extension_value->SetString("id", id);
    extension_value->SetString("version", extension->VersionString());
    extension_value->SetString("name", extension->name());
     extension_value->SetString("public_key", extension->public_key());
     extension_value->SetString("description", extension->description());
     extension_value->SetString("background_url",
                               extension->GetBackgroundURL().spec());
     extension_value->SetString("options_url",
                                extension->options_url().spec());
     extension_value->Set("host_permissions",
                         GetHostPermissions(extension, false));
    extension_value->Set("effective_host_permissions",
                         GetHostPermissions(extension, true));
    extension_value->Set("api_permissions", GetAPIPermissions(extension));
    Extension::Location location = extension->location();
    extension_value->SetBoolean("is_component",
                                location == Extension::COMPONENT);
    extension_value->SetBoolean("is_internal", location == Extension::INTERNAL);
    extension_value->SetBoolean("is_user_installed",
        location == Extension::INTERNAL || location == Extension::LOAD);
    extension_value->SetBoolean("is_enabled", service->IsExtensionEnabled(id));
    extension_value->SetBoolean("allowed_in_incognito",
                                service->IsIncognitoEnabled(id));
    extension_value->SetBoolean("has_page_action",
                                extension->page_action() != NULL);
    extensions_values->Append(extension_value);
  }
  return_value->Set("extensions", extensions_values);
  reply.SendSuccess(return_value.get());
}
