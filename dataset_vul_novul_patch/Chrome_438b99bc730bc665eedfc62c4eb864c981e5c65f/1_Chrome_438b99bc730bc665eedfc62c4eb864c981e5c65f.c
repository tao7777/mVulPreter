void AppShortcutManager::Observe(int type,
                                 const content::NotificationSource& source,
                                 const content::NotificationDetails& details) {
  switch (type) {
    case chrome::NOTIFICATION_EXTENSIONS_READY: {
      OnceOffCreateShortcuts();
       break;
     }
     case chrome::NOTIFICATION_EXTENSION_INSTALLED_DEPRECATED: {
#if defined(OS_MACOSX)
      if (!apps::IsAppShimsEnabled())
        break;
#endif  // defined(OS_MACOSX)
       const extensions::InstalledExtensionInfo* installed_info =
           content::Details<const extensions::InstalledExtensionInfo>(details)
               .ptr();
      const Extension* extension = installed_info->extension;
      if (installed_info->is_update) {
        web_app::UpdateAllShortcuts(
            base::UTF8ToUTF16(installed_info->old_name), profile_, extension);
      } else if (ShouldCreateShortcutFor(profile_, extension)) {
        CreateShortcutsInApplicationsMenu(profile_, extension);
      }
      break;
    }
    case chrome::NOTIFICATION_EXTENSION_UNINSTALLED: {
      const Extension* extension = content::Details<const Extension>(
          details).ptr();
      web_app::DeleteAllShortcuts(profile_, extension);
      break;
    }
    default:
      NOTREACHED();
  }
}
