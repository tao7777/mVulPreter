void Browser::Observe(NotificationType type,
                      const NotificationSource& source,
                      const NotificationDetails& details) {
   switch (type.value) {
     case NotificationType::TAB_CONTENTS_DISCONNECTED:
       if (is_attempting_to_close_browser_) {
        MessageLoop::current()->PostTask(
            FROM_HERE,
            method_factory_.NewRunnableMethod(&Browser::ClearUnloadState,
                Source<TabContents>(source).ptr()));
       }
       break;
 
    case NotificationType::SSL_VISIBLE_STATE_CHANGED:
      if (GetSelectedTabContents() &&
          &GetSelectedTabContents()->controller() ==
          Source<NavigationController>(source).ptr())
        UpdateToolbar(false);
      break;

    case NotificationType::EXTENSION_UPDATE_DISABLED: {
      Profile* profile = Source<Profile>(source).ptr();
      if (profile_->IsSameProfile(profile)) {
        ExtensionService* service = profile->GetExtensionService();
        DCHECK(service);
        const Extension* extension = Details<const Extension>(details).ptr();
        if (service->extension_prefs()->DidExtensionEscalatePermissions(
                extension->id()))
          ShowExtensionDisabledUI(service, profile_, extension);
      }
      break;
    }

    case NotificationType::EXTENSION_UNLOADED: {
      window()->GetLocationBar()->UpdatePageActions();

      const Extension* extension =
          Details<UnloadedExtensionInfo>(details)->extension;
      TabStripModel* model = tab_handler_->GetTabStripModel();
      for (int i = model->count() - 1; i >= 0; --i) {
        TabContents* tc = model->GetTabContentsAt(i)->tab_contents();
        if (tc->GetURL().SchemeIs(chrome::kExtensionScheme) &&
            tc->GetURL().host() == extension->id()) {
          CloseTabContents(tc);
        }
      }

      break;
    }

    case NotificationType::EXTENSION_PROCESS_TERMINATED: {
      window()->GetLocationBar()->InvalidatePageActions();

      TabContents* tab_contents = GetSelectedTabContents();
      if (!tab_contents)
        break;
      ExtensionService* extensions_service =
          Source<Profile>(source).ptr()->GetExtensionService();
      ExtensionHost* extension_host = Details<ExtensionHost>(details).ptr();
      tab_contents->AddInfoBar(new CrashedExtensionInfoBarDelegate(
          tab_contents, extensions_service, extension_host->extension()));
      break;
    }

    case NotificationType::EXTENSION_LOADED: {
      window()->GetLocationBar()->UpdatePageActions();

      const Extension* extension = Details<const Extension>(details).ptr();
      CrashedExtensionInfoBarDelegate* delegate = NULL;
      TabStripModel* model = tab_handler_->GetTabStripModel();
      for (int m = 0; m < model->count(); ++m) {
        TabContents* tab_contents = model->GetTabContentsAt(m)->tab_contents();
        for (int i = 0; i < tab_contents->infobar_delegate_count();) {
          delegate = tab_contents->GetInfoBarDelegateAt(i)->
              AsCrashedExtensionInfoBarDelegate();
          if (delegate && delegate->extension_id() == extension->id()) {
            tab_contents->RemoveInfoBar(delegate);
            continue;
          }
          ++i;
        }
      }
      break;
    }

    case NotificationType::BROWSER_THEME_CHANGED:
      window()->UserChangedTheme();
      break;

    case NotificationType::EXTENSION_READY_FOR_INSTALL: {
      if (BrowserList::FindBrowserWithType(profile(),
                                           Browser::TYPE_NORMAL,
                                           true) != this)
        break;

      GURL download_url = *(Details<GURL>(details).ptr());
      if (ExtensionService::IsDownloadFromMiniGallery(download_url))
        window()->ShowThemeInstallBubble();
      break;
    }

    case NotificationType::PROFILE_ERROR: {
      if (BrowserList::GetLastActive() != this)
        break;
      int* message_id = Details<int>(details).ptr();
      window()->ShowProfileErrorDialog(*message_id);
      break;
    }

    case NotificationType::PREF_CHANGED: {
      const std::string& pref_name = *Details<std::string>(details).ptr();
      if (pref_name == prefs::kUseVerticalTabs) {
        UseVerticalTabsChanged();
      } else if (pref_name == prefs::kPrintingEnabled) {
        UpdatePrintingState(0);
      } else if (pref_name == prefs::kInstantEnabled) {
        if (!InstantController::IsEnabled(profile())) {
          if (instant()) {
            instant()->DestroyPreviewContents();
            instant_.reset();
            instant_unload_handler_.reset();
          }
        } else {
          CreateInstantIfNecessary();
        }
      } else if (pref_name == prefs::kDevToolsDisabled) {
        UpdateCommandsForDevTools();
        if (dev_tools_disabled_.GetValue())
          g_browser_process->devtools_manager()->CloseAllClientHosts();
      } else {
        NOTREACHED();
      }
      break;
    }

    default:
      NOTREACHED() << "Got a notification we didn't register for.";
  }
}
