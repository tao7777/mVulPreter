void BackgroundContentsService::LoadBackgroundContentsFromManifests(
    Profile* profile) {
  const ExtensionSet* extensions =
      profile->GetExtensionService()->extensions();
   ExtensionSet::const_iterator iter = extensions->begin();
   for (; iter != extensions->end(); ++iter) {
     const Extension* extension = *iter;
    if (extension->is_hosted_app() && extension->has_background_page()) {
       LoadBackgroundContents(profile,
                             extension->GetBackgroundURL(),
                              ASCIIToUTF16("background"),
                              UTF8ToUTF16(extension->id()));
     }
  }
}
