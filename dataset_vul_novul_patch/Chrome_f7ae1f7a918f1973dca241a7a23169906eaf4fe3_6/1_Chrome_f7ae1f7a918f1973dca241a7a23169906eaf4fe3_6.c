static void WillDispatchTabUpdatedEvent(WebContents* contents,
                                        Profile* profile,
                                        const Extension* extension,
                                        ListValue* event_args) {
   DictionaryValue* tab_value = ExtensionTabUtil::CreateTabValue(
       contents, extension);
 }
