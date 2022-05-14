void ExtensionContextMenuModel::InitCommonCommands() {
  const Extension* extension = GetExtension();

  DCHECK(extension);

  AddItem(NAME, UTF8ToUTF16(extension->name()));
   AddSeparator();
   AddItemWithStringId(CONFIGURE, IDS_EXTENSIONS_OPTIONS);
   AddItemWithStringId(DISABLE, IDS_EXTENSIONS_DISABLE);
  AddItem(UNINSTALL, l10n_util::GetStringFUTF16(IDS_EXTENSIONS_UNINSTALL,
      l10n_util::GetStringUTF16(IDS_SHORT_PRODUCT_NAME)));
   if (extension->browser_action())
     AddItemWithStringId(HIDE, IDS_EXTENSIONS_HIDE_BUTTON);
   AddSeparator();
  AddItemWithStringId(MANAGE, IDS_MANAGE_EXTENSIONS);
}
