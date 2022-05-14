void BackgroundContentsService::Observe(
    int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) {
  switch (type) {
    case chrome::NOTIFICATION_EXTENSIONS_READY:
      LoadBackgroundContentsFromManifests(
          content::Source<Profile>(source).ptr());
      LoadBackgroundContentsFromPrefs(content::Source<Profile>(source).ptr());
      break;
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_DELETED:
      BackgroundContentsShutdown(
          content::Details<BackgroundContents>(details).ptr());
      break;
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_CLOSED:
      DCHECK(IsTracked(content::Details<BackgroundContents>(details).ptr()));
      UnregisterBackgroundContents(
          content::Details<BackgroundContents>(details).ptr());
      break;
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_NAVIGATED: {
      DCHECK(IsTracked(content::Details<BackgroundContents>(details).ptr()));

      BackgroundContents* bgcontents =
          content::Details<BackgroundContents>(details).ptr();
      Profile* profile = content::Source<Profile>(source).ptr();
      const string16& appid = GetParentApplicationId(bgcontents);
      ExtensionService* extension_service = profile->GetExtensionService();
       if (extension_service) {
         const Extension* extension =
             extension_service->GetExtensionById(UTF16ToUTF8(appid), false);
        if (extension && extension->has_background_page())
           break;
       }
       RegisterBackgroundContents(bgcontents);
      break;
    }
    case chrome::NOTIFICATION_EXTENSION_LOADED: {
      const Extension* extension =
           content::Details<const Extension>(details).ptr();
       Profile* profile = content::Source<Profile>(source).ptr();
       if (extension->is_hosted_app() &&
          extension->has_background_page()) {
         ShutdownAssociatedBackgroundContents(ASCIIToUTF16(extension->id()));

        ExtensionService* service = profile->GetExtensionService();
        if (service && service->is_ready()) {
          LoadBackgroundContents(profile, extension->GetBackgroundURL(),
               ASCIIToUTF16("background"), UTF8ToUTF16(extension->id()));
         }
       }

      ScheduleCloseBalloon(extension->id());
      break;
    }
    case chrome::NOTIFICATION_EXTENSION_PROCESS_TERMINATED:
    case chrome::NOTIFICATION_BACKGROUND_CONTENTS_TERMINATED: {
      Profile* profile = content::Source<Profile>(source).ptr();
      const Extension* extension = NULL;
      if (type == chrome::NOTIFICATION_BACKGROUND_CONTENTS_TERMINATED) {
        BackgroundContents* bg =
            content::Details<BackgroundContents>(details).ptr();
        std::string extension_id = UTF16ToASCII(
            BackgroundContentsServiceFactory::GetForProfile(profile)->
                GetParentApplicationId(bg));
        extension =
          profile->GetExtensionService()->GetExtensionById(extension_id, false);
      } else {
        ExtensionHost* extension_host =
            content::Details<ExtensionHost>(details).ptr();
        extension = extension_host->extension();
      }
      if (!extension)
        break;

      MessageLoop::current()->PostTask(
          FROM_HERE, base::Bind(&ShowBalloon, extension, profile));
      break;
    }
    case chrome::NOTIFICATION_EXTENSION_UNLOADED:
      switch (content::Details<UnloadedExtensionInfo>(details)->reason) {
        case extension_misc::UNLOAD_REASON_DISABLE:    // Fall through.
        case extension_misc::UNLOAD_REASON_TERMINATE:  // Fall through.
        case extension_misc::UNLOAD_REASON_UNINSTALL:
          ShutdownAssociatedBackgroundContents(
              ASCIIToUTF16(content::Details<UnloadedExtensionInfo>(details)->
                  extension->id()));
          break;
        case extension_misc::UNLOAD_REASON_UPDATE: {
           const Extension* extension =
               content::Details<UnloadedExtensionInfo>(details)->extension;
          if (extension->has_background_page())
             ShutdownAssociatedBackgroundContents(ASCIIToUTF16(extension->id()));
           break;
         }
        default:
          NOTREACHED();
          ShutdownAssociatedBackgroundContents(
              ASCIIToUTF16(content::Details<UnloadedExtensionInfo>(details)->
                  extension->id()));
          break;
      }
      break;

    case chrome::NOTIFICATION_EXTENSION_UNINSTALLED: {
      ScheduleCloseBalloon(*content::Details<const std::string>(details).ptr());
      break;
    }

    default:
      NOTREACHED();
      break;
  }
}
