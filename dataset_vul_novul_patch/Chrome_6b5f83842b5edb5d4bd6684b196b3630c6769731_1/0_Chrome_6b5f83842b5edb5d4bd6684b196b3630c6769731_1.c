string16 ExtensionInstallUI::Prompt::GetDialogTitle(
    const Extension* extension) const {
  if (type_ == INSTALL_PROMPT) {
     return l10n_util::GetStringUTF16(extension->is_app() ?
         IDS_EXTENSION_INSTALL_APP_PROMPT_TITLE :
         IDS_EXTENSION_INSTALL_EXTENSION_PROMPT_TITLE);
   } else {
     return l10n_util::GetStringUTF16(kTitleIds[type_]);
   }
}
