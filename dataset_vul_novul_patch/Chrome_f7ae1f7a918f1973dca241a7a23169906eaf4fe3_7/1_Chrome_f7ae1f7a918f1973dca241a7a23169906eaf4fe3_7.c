DictionaryValue* ExtensionTabUtil::CreateTabValue(
    const WebContents* contents,
     TabStripModel* tab_strip,
     int tab_index,
     const Extension* extension) {
  bool has_permission = extension && extension->HasAPIPermissionForTab(
      GetTabId(contents), APIPermission::kTab);
  return CreateTabValue(contents, tab_strip, tab_index,
                        has_permission ? INCLUDE_PRIVACY_SENSITIVE_FIELDS :
                            OMIT_PRIVACY_SENSITIVE_FIELDS);
 }
