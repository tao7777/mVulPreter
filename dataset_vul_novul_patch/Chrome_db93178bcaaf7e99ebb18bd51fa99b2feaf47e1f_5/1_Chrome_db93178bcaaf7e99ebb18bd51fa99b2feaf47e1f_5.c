AppResult::AppResult(Profile* profile,
                     const std::string& app_id,
                     AppListControllerDelegate* controller,
                     bool is_recommendation)
    : profile_(profile),
      app_id_(app_id),
      controller_(controller),
      extension_registry_(NULL) {
  set_id(extensions::Extension::GetBaseURLFromExtensionId(app_id_).spec());
  if (app_list::switches::IsExperimentalAppListEnabled())
     set_display_type(is_recommendation ? DISPLAY_RECOMMENDATION : DISPLAY_TILE);
 
   const extensions::Extension* extension =
      extensions::ExtensionSystem::Get(profile_)->extension_service()
          ->GetInstalledExtension(app_id_);
   DCHECK(extension);
 
   is_platform_app_ = extension->is_platform_app();

  icon_.reset(
      new extensions::IconImage(profile_,
                                extension,
                                extensions::IconsInfo::GetIcons(extension),
                                GetPreferredIconDimension(),
                                extensions::util::GetDefaultAppIcon(),
                                this));
  UpdateIcon();

  StartObservingExtensionRegistry();
}
