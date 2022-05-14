bool BrowserCommandController::ExecuteCommandWithDisposition(
    int id, WindowOpenDisposition disposition) {
  if (!SupportsCommand(id) || !IsCommandEnabled(id))
    return false;

  if (browser_->tab_strip_model()->active_index() == TabStripModel::kNoTab)
    return true;

  DCHECK(command_updater_.IsCommandEnabled(id)) << "Invalid/disabled command "
                                                << id;

  switch (id) {
    case IDC_BACK:
      GoBack(browser_, disposition);
      break;
    case IDC_FORWARD:
      GoForward(browser_, disposition);
      break;
    case IDC_RELOAD:
      Reload(browser_, disposition);
      break;
    case IDC_RELOAD_CLEARING_CACHE:
      ClearCache(browser_);
      FALLTHROUGH;
    case IDC_RELOAD_BYPASSING_CACHE:
      ReloadBypassingCache(browser_, disposition);
      break;
    case IDC_HOME:
      Home(browser_, disposition);
      break;
    case IDC_OPEN_CURRENT_URL:
      OpenCurrentURL(browser_);
      break;
    case IDC_STOP:
      Stop(browser_);
      break;

    case IDC_NEW_WINDOW:
      NewWindow(browser_);
      break;
    case IDC_NEW_INCOGNITO_WINDOW:
      NewIncognitoWindow(profile());
      break;
    case IDC_CLOSE_WINDOW:
      base::RecordAction(base::UserMetricsAction("CloseWindowByKey"));
      CloseWindow(browser_);
      break;
    case IDC_NEW_TAB: {
      NewTab(browser_);
#if BUILDFLAG(ENABLE_DESKTOP_IN_PRODUCT_HELP)
      auto* new_tab_tracker =
          feature_engagement::NewTabTrackerFactory::GetInstance()
              ->GetForProfile(profile());

      new_tab_tracker->OnNewTabOpened();
      new_tab_tracker->CloseBubble();
#endif
      break;
    }
    case IDC_CLOSE_TAB:
      base::RecordAction(base::UserMetricsAction("CloseTabByKey"));
      CloseTab(browser_);
      break;
    case IDC_SELECT_NEXT_TAB:
      base::RecordAction(base::UserMetricsAction("Accel_SelectNextTab"));
      SelectNextTab(browser_);
      break;
    case IDC_SELECT_PREVIOUS_TAB:
      base::RecordAction(base::UserMetricsAction("Accel_SelectPreviousTab"));
      SelectPreviousTab(browser_);
      break;
    case IDC_MOVE_TAB_NEXT:
      MoveTabNext(browser_);
      break;
    case IDC_MOVE_TAB_PREVIOUS:
      MoveTabPrevious(browser_);
      break;
    case IDC_SELECT_TAB_0:
    case IDC_SELECT_TAB_1:
    case IDC_SELECT_TAB_2:
    case IDC_SELECT_TAB_3:
    case IDC_SELECT_TAB_4:
    case IDC_SELECT_TAB_5:
    case IDC_SELECT_TAB_6:
    case IDC_SELECT_TAB_7:
      base::RecordAction(base::UserMetricsAction("Accel_SelectNumberedTab"));
      SelectNumberedTab(browser_, id - IDC_SELECT_TAB_0);
      break;
    case IDC_SELECT_LAST_TAB:
      base::RecordAction(base::UserMetricsAction("Accel_SelectNumberedTab"));
      SelectLastTab(browser_);
      break;
    case IDC_DUPLICATE_TAB:
      DuplicateTab(browser_);
      break;
    case IDC_RESTORE_TAB:
      RestoreTab(browser_);
      break;
    case IDC_SHOW_AS_TAB:
      ConvertPopupToTabbedBrowser(browser_);
      break;
    case IDC_FULLSCREEN:
      chrome::ToggleFullscreenMode(browser_);
      break;
    case IDC_OPEN_IN_PWA_WINDOW:
      base::RecordAction(base::UserMetricsAction("OpenActiveTabInPwaWindow"));
      ReparentSecureActiveTabIntoPwaWindow(browser_);
      break;

#if defined(OS_CHROMEOS)
    case IDC_VISIT_DESKTOP_OF_LRU_USER_2:
    case IDC_VISIT_DESKTOP_OF_LRU_USER_3:
      ExecuteVisitDesktopCommand(id, window()->GetNativeWindow());
      break;
#endif

#if defined(OS_LINUX) && !defined(OS_CHROMEOS)
    case IDC_MINIMIZE_WINDOW:
      browser_->window()->Minimize();
      break;
    case IDC_MAXIMIZE_WINDOW:
      browser_->window()->Maximize();
      break;
    case IDC_RESTORE_WINDOW:
      browser_->window()->Restore();
      break;
    case IDC_USE_SYSTEM_TITLE_BAR: {
      PrefService* prefs = profile()->GetPrefs();
      prefs->SetBoolean(prefs::kUseCustomChromeFrame,
                        !prefs->GetBoolean(prefs::kUseCustomChromeFrame));
      break;
    }
#endif

#if defined(OS_MACOSX)
    case IDC_TOGGLE_FULLSCREEN_TOOLBAR:
       chrome::ToggleFullscreenToolbar(browser_);
       break;
     case IDC_TOGGLE_JAVASCRIPT_APPLE_EVENTS: {
      chrome::ToggleJavaScriptFromAppleEventsAllowed(browser_);
       break;
     }
 #endif
    case IDC_EXIT:
      Exit();
      break;

    case IDC_SAVE_PAGE:
      SavePage(browser_);
      break;
    case IDC_BOOKMARK_PAGE:
#if BUILDFLAG(ENABLE_DESKTOP_IN_PRODUCT_HELP)
      feature_engagement::BookmarkTrackerFactory::GetInstance()
          ->GetForProfile(profile())
          ->OnBookmarkAdded();
#endif
      BookmarkCurrentPageAllowingExtensionOverrides(browser_);
      break;
    case IDC_BOOKMARK_ALL_TABS:
#if BUILDFLAG(ENABLE_DESKTOP_IN_PRODUCT_HELP)
      feature_engagement::BookmarkTrackerFactory::GetInstance()
          ->GetForProfile(profile())
          ->OnBookmarkAdded();
#endif
      BookmarkAllTabs(browser_);
      break;
    case IDC_VIEW_SOURCE:
      browser_->tab_strip_model()
          ->GetActiveWebContents()
          ->GetMainFrame()
          ->ViewSource();
      break;
    case IDC_EMAIL_PAGE_LOCATION:
      EmailPageLocation(browser_);
      break;
    case IDC_PRINT:
      Print(browser_);
      break;

#if BUILDFLAG(ENABLE_PRINTING)
    case IDC_BASIC_PRINT:
      base::RecordAction(base::UserMetricsAction("Accel_Advanced_Print"));
      BasicPrint(browser_);
      break;
#endif  // ENABLE_PRINTING

    case IDC_SAVE_CREDIT_CARD_FOR_PAGE:
      SaveCreditCard(browser_);
      break;
    case IDC_MIGRATE_LOCAL_CREDIT_CARD_FOR_PAGE:
      MigrateLocalCards(browser_);
      break;
    case IDC_TRANSLATE_PAGE:
      Translate(browser_);
      break;
    case IDC_MANAGE_PASSWORDS_FOR_PAGE:
      ManagePasswordsForPage(browser_);
      break;

    case IDC_CUT:
    case IDC_COPY:
    case IDC_PASTE:
      CutCopyPaste(browser_, id);
      break;

    case IDC_FIND:
      Find(browser_);
      break;
    case IDC_FIND_NEXT:
      FindNext(browser_);
      break;
    case IDC_FIND_PREVIOUS:
      FindPrevious(browser_);
      break;

    case IDC_ZOOM_PLUS:
      Zoom(browser_, content::PAGE_ZOOM_IN);
      break;
    case IDC_ZOOM_NORMAL:
      Zoom(browser_, content::PAGE_ZOOM_RESET);
      break;
    case IDC_ZOOM_MINUS:
      Zoom(browser_, content::PAGE_ZOOM_OUT);
      break;

    case IDC_FOCUS_TOOLBAR:
      base::RecordAction(base::UserMetricsAction("Accel_Focus_Toolbar"));
      FocusToolbar(browser_);
      break;
    case IDC_FOCUS_LOCATION:
      base::RecordAction(base::UserMetricsAction("Accel_Focus_Location"));
      FocusLocationBar(browser_);
      break;
    case IDC_FOCUS_SEARCH:
      base::RecordAction(base::UserMetricsAction("Accel_Focus_Search"));
      FocusSearch(browser_);
      break;
    case IDC_FOCUS_MENU_BAR:
      FocusAppMenu(browser_);
      break;
    case IDC_FOCUS_BOOKMARKS:
      base::RecordAction(base::UserMetricsAction("Accel_Focus_Bookmarks"));
      FocusBookmarksToolbar(browser_);
      break;
    case IDC_FOCUS_INACTIVE_POPUP_FOR_ACCESSIBILITY:
      FocusInactivePopupForAccessibility(browser_);
      break;
    case IDC_FOCUS_NEXT_PANE:
      FocusNextPane(browser_);
      break;
    case IDC_FOCUS_PREVIOUS_PANE:
      FocusPreviousPane(browser_);
      break;

    case IDC_OPEN_FILE:
      browser_->OpenFile();
      break;
    case IDC_CREATE_SHORTCUT:
      CreateBookmarkAppFromCurrentWebContents(browser_,
                                              true /* force_shortcut_app */);
      break;
    case IDC_INSTALL_PWA:
      CreateBookmarkAppFromCurrentWebContents(browser_,
                                              false /* force_shortcut_app */);
      break;
    case IDC_DEV_TOOLS:
      ToggleDevToolsWindow(browser_, DevToolsToggleAction::Show());
      break;
    case IDC_DEV_TOOLS_CONSOLE:
      ToggleDevToolsWindow(browser_, DevToolsToggleAction::ShowConsolePanel());
      break;
    case IDC_DEV_TOOLS_DEVICES:
      InspectUI::InspectDevices(browser_);
      break;
    case IDC_DEV_TOOLS_INSPECT:
      ToggleDevToolsWindow(browser_, DevToolsToggleAction::Inspect());
      break;
    case IDC_DEV_TOOLS_TOGGLE:
      ToggleDevToolsWindow(browser_, DevToolsToggleAction::Toggle());
      break;
    case IDC_TASK_MANAGER:
      OpenTaskManager(browser_);
      break;
#if defined(OS_CHROMEOS)
    case IDC_TAKE_SCREENSHOT:
      TakeScreenshot();
      break;
#endif
#if defined(GOOGLE_CHROME_BUILD)
    case IDC_FEEDBACK:
      OpenFeedbackDialog(browser_, kFeedbackSourceBrowserCommand);
      break;
#endif
    case IDC_SHOW_BOOKMARK_BAR:
      ToggleBookmarkBar(browser_);
      break;
    case IDC_PROFILING_ENABLED:
      Profiling::Toggle();
      break;

    case IDC_SHOW_BOOKMARK_MANAGER:
      ShowBookmarkManager(browser_);
      break;
    case IDC_SHOW_APP_MENU:
      base::RecordAction(base::UserMetricsAction("Accel_Show_App_Menu"));
      ShowAppMenu(browser_);
      break;
    case IDC_SHOW_AVATAR_MENU:
      ShowAvatarMenu(browser_);
      break;
    case IDC_SHOW_HISTORY:
      ShowHistory(browser_);
      break;
    case IDC_SHOW_DOWNLOADS:
      ShowDownloads(browser_);
      break;
    case IDC_MANAGE_EXTENSIONS:
      ShowExtensions(browser_, std::string());
      break;
    case IDC_OPTIONS:
      ShowSettings(browser_);
      break;
    case IDC_EDIT_SEARCH_ENGINES:
      ShowSearchEngineSettings(browser_);
      break;
    case IDC_VIEW_PASSWORDS:
      ShowPasswordManager(browser_);
      break;
    case IDC_CLEAR_BROWSING_DATA:
      ShowClearBrowsingDataDialog(browser_);
      break;
    case IDC_IMPORT_SETTINGS:
      ShowImportDialog(browser_);
      break;
    case IDC_TOGGLE_REQUEST_TABLET_SITE:
      ToggleRequestTabletSite(browser_);
      break;
    case IDC_ABOUT:
      ShowAboutChrome(browser_);
      break;
    case IDC_UPGRADE_DIALOG:
      OpenUpdateChromeDialog(browser_);
      break;
    case IDC_HELP_PAGE_VIA_KEYBOARD:
      ShowHelp(browser_, HELP_SOURCE_KEYBOARD);
      break;
    case IDC_HELP_PAGE_VIA_MENU:
      ShowHelp(browser_, HELP_SOURCE_MENU);
      break;
    case IDC_SHOW_BETA_FORUM:
      ShowBetaForum(browser_);
      break;
    case IDC_SHOW_SIGNIN:
      ShowBrowserSigninOrSettings(
          browser_, signin_metrics::AccessPoint::ACCESS_POINT_MENU);
      break;
    case IDC_DISTILL_PAGE:
      DistillCurrentPage(browser_);
      break;
    case IDC_ROUTE_MEDIA:
      RouteMedia(browser_);
      break;
    case IDC_WINDOW_MUTE_SITE:
      MuteSite(browser_);
      break;
    case IDC_WINDOW_PIN_TAB:
      PinTab(browser_);
      break;

    case IDC_COPY_URL:
      CopyURL(browser_);
      break;
    case IDC_OPEN_IN_CHROME:
      OpenInChrome(browser_);
      break;
    case IDC_SITE_SETTINGS:
      ShowSiteSettings(
          browser_,
          browser_->tab_strip_model()->GetActiveWebContents()->GetVisibleURL());
      break;
    case IDC_HOSTED_APP_MENU_APP_INFO:
      ShowPageInfoDialog(browser_->tab_strip_model()->GetActiveWebContents(),
                         bubble_anchor_util::kAppMenuButton);
      break;

    default:
      LOG(WARNING) << "Received Unimplemented Command: " << id;
      break;
  }

  return true;
}
