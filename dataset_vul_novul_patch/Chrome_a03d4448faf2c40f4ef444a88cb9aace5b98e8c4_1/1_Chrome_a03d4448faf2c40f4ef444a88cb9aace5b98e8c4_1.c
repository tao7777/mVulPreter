void BackgroundContentsService::LoadBackgroundContentsFromManifests(
    Profile* profile) {
  const ExtensionSet* extensions =
      profile->GetExtensionService()->extensions();
   ExtensionSet::const_iterator iter = extensions->begin();
   for (; iter != extensions->end(); ++iter) {
     const Extension* extension = *iter;
    if (extension->is_hosted_app() &&
        extension->background_url().is_valid()) {
       LoadBackgroundContents(profile,
                             extension->background_url(),
                              ASCIIToUTF16("background"),
                              UTF8ToUTF16(extension->id()));
     }
  }
}
