void BackgroundContentsService::LoadBackgroundContentsForExtension(
    Profile* profile,
    const std::string& extension_id) {
   const Extension* extension =
       profile->GetExtensionService()->GetExtensionById(extension_id, false);
   DCHECK(!extension || extension->is_hosted_app());
  if (extension && extension->background_url().is_valid()) {
     LoadBackgroundContents(profile,
                           extension->background_url(),
                            ASCIIToUTF16("background"),
                            UTF8ToUTF16(extension->id()));
     return;
  }

  if (!prefs_)
    return;
  const DictionaryValue* contents =
      prefs_->GetDictionary(prefs::kRegisteredBackgroundContents);
  if (!contents)
    return;
  LoadBackgroundContentsFromDictionary(profile, extension_id, contents);
}
