static void WillDispatchTabUpdatedEvent(WebContents* contents,
static void WillDispatchTabUpdatedEvent(
    WebContents* contents,
    const DictionaryValue* changed_properties,
    Profile* profile,
    const Extension* extension,
    ListValue* event_args) {
  // Overwrite the second argument with the appropriate properties dictionary,
  // depending on extension permissions.
  DictionaryValue* properties_value = changed_properties->DeepCopy();
  ExtensionTabUtil::ScrubTabValueForExtension(contents, extension,
                                              properties_value);
  event_args->Set(1, properties_value);

  // Overwrite the third arg with our tab value as seen by this extension.
   DictionaryValue* tab_value = ExtensionTabUtil::CreateTabValue(
       contents, extension);
 }
