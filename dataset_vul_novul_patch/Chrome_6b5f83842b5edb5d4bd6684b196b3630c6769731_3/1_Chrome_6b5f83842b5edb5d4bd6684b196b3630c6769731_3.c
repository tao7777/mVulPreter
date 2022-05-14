PanelSettingsMenuModel::PanelSettingsMenuModel(Panel* panel)
    : ALLOW_THIS_IN_INITIALIZER_LIST(ui::SimpleMenuModel(this)),
      panel_(panel) {
  const Extension* extension = panel_->GetExtension();
  DCHECK(extension);

  AddItem(COMMAND_NAME, UTF8ToUTF16(extension->name()));
  AddSeparator();
   AddItem(COMMAND_CONFIGURE, l10n_util::GetStringUTF16(IDS_EXTENSIONS_OPTIONS));
   AddItem(COMMAND_DISABLE, l10n_util::GetStringUTF16(IDS_EXTENSIONS_DISABLE));
   AddItem(COMMAND_UNINSTALL,
      l10n_util::GetStringFUTF16(IDS_EXTENSIONS_UNINSTALL,
          l10n_util::GetStringUTF16(IDS_SHORT_PRODUCT_NAME)));
   AddSeparator();
   AddItem(COMMAND_MANAGE, l10n_util::GetStringUTF16(IDS_MANAGE_EXTENSIONS));
 }
