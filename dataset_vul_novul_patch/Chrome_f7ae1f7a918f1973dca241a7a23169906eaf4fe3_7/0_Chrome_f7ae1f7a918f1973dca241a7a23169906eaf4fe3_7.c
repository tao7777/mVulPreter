DictionaryValue* ExtensionTabUtil::CreateTabValue(
    const WebContents* contents,
     TabStripModel* tab_strip,
     int tab_index,
     const Extension* extension) {
  DictionaryValue *result = CreateTabValue(contents, tab_strip, tab_index);
  ScrubTabValueForExtension(contents, extension, result);
  return result;
 }
