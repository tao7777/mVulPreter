 void BeginInstallWithManifestFunction::OnParseSuccess(
     const SkBitmap& icon, DictionaryValue* parsed_manifest) {
  CHECK(parsed_manifest);
  icon_ = icon;
  parsed_manifest_.reset(parsed_manifest);

  std::string init_errors;
  dummy_extension_ = Extension::Create(
      FilePath(),
      Extension::INTERNAL,
      *static_cast<DictionaryValue*>(parsed_manifest_.get()),
      Extension::NO_FLAGS,
      &init_errors);
  if (!dummy_extension_.get()) {
    OnParseFailure(MANIFEST_ERROR, std::string(kInvalidManifestError));
    return;
  }
   if (icon_.empty())
     icon_ = Extension::GetDefaultIcon(dummy_extension_->is_app());
 
   ShowExtensionInstallDialog(profile(),
                              this,
                              dummy_extension_.get(),
                             &icon_,
                             dummy_extension_->GetPermissionMessageStrings(),
                             ExtensionInstallUI::INSTALL_PROMPT);

}
