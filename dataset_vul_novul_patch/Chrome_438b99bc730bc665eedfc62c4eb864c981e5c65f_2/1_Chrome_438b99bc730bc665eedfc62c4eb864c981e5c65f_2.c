void NTPResourceCache::CreateNewTabHTML() {
  PrefService* prefs = profile_->GetPrefs();
  base::DictionaryValue load_time_data;
  load_time_data.SetBoolean("bookmarkbarattached",
      prefs->GetBoolean(prefs::kShowBookmarkBar));
  load_time_data.SetBoolean("hasattribution",
      ThemeServiceFactory::GetForProfile(profile_)->HasCustomImage(
          IDR_THEME_NTP_ATTRIBUTION));
  load_time_data.SetBoolean("showMostvisited", should_show_most_visited_page_);
  load_time_data.SetBoolean("showAppLauncherPromo",
      ShouldShowAppLauncherPromo());
  load_time_data.SetBoolean("showRecentlyClosed",
      should_show_recently_closed_menu_);
  load_time_data.SetString("title",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_TITLE));
  load_time_data.SetString("mostvisited",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_MOST_VISITED));
  load_time_data.SetString("suggestions",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_SUGGESTIONS));
  load_time_data.SetString("restoreThumbnailsShort",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RESTORE_THUMBNAILS_SHORT_LINK));
  load_time_data.SetString("recentlyclosed",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RECENTLY_CLOSED));
  load_time_data.SetString("webStoreTitle",
      l10n_util::GetStringUTF16(IDS_EXTENSION_WEB_STORE_TITLE));
  load_time_data.SetString("webStoreTitleShort",
      l10n_util::GetStringUTF16(IDS_EXTENSION_WEB_STORE_TITLE_SHORT));
  load_time_data.SetString("closedwindowsingle",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RECENTLY_CLOSED_WINDOW_SINGLE));
  load_time_data.SetString("closedwindowmultiple",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RECENTLY_CLOSED_WINDOW_MULTIPLE));
  load_time_data.SetString("attributionintro",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_ATTRIBUTION_INTRO));
  load_time_data.SetString("thumbnailremovednotification",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_THUMBNAIL_REMOVED_NOTIFICATION));
  load_time_data.SetString("undothumbnailremove",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_UNDO_THUMBNAIL_REMOVE));
  load_time_data.SetString("removethumbnailtooltip",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_REMOVE_THUMBNAIL_TOOLTIP));
  load_time_data.SetString("appuninstall",
      l10n_util::GetStringUTF16(IDS_EXTENSIONS_UNINSTALL));
  load_time_data.SetString("appoptions",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_OPTIONS));
  load_time_data.SetString("appdetails",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_DETAILS));
  load_time_data.SetString("appcreateshortcut",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_CREATE_SHORTCUT));
  load_time_data.SetString("appDefaultPageName",
      l10n_util::GetStringUTF16(IDS_APP_DEFAULT_PAGE_NAME));
  load_time_data.SetString("applaunchtypepinned",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_PINNED));
  load_time_data.SetString("applaunchtyperegular",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_REGULAR));
  load_time_data.SetString("applaunchtypewindow",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_WINDOW));
  load_time_data.SetString("applaunchtypefullscreen",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_FULLSCREEN));
  load_time_data.SetString("syncpromotext",
      l10n_util::GetStringUTF16(IDS_SYNC_START_SYNC_BUTTON_LABEL));
  load_time_data.SetString("syncLinkText",
      l10n_util::GetStringUTF16(IDS_SYNC_ADVANCED_OPTIONS));
  load_time_data.SetBoolean("shouldShowSyncLogin",
                            NTPLoginHandler::ShouldShow(profile_));
  load_time_data.SetString("otherSessions",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_OTHER_SESSIONS_LABEL));
  load_time_data.SetString("otherSessionsEmpty",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_OTHER_SESSIONS_EMPTY));
  load_time_data.SetString("otherSessionsLearnMoreUrl",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_OTHER_SESSIONS_LEARN_MORE_URL));
  load_time_data.SetString("learnMore",
      l10n_util::GetStringUTF16(IDS_LEARN_MORE));
  load_time_data.SetString("webStoreLink",
      GetUrlWithLang(GURL(extension_urls::GetWebstoreLaunchURL())));
  load_time_data.SetString("appInstallHintText",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_INSTALL_HINT_LABEL));
  load_time_data.SetBoolean("isDiscoveryInNTPEnabled",
      NewTabUI::IsDiscoveryInNTPEnabled());
  load_time_data.SetString("collapseSessionMenuItemText",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_OTHER_SESSIONS_COLLAPSE_SESSION));
  load_time_data.SetString("expandSessionMenuItemText",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_OTHER_SESSIONS_EXPAND_SESSION));
  load_time_data.SetString("restoreSessionMenuItemText",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_OTHER_SESSIONS_OPEN_ALL));
  load_time_data.SetString("learn_more",
      l10n_util::GetStringUTF16(IDS_LEARN_MORE));
  load_time_data.SetString("tile_grid_screenreader_accessible_description",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_TILE_GRID_ACCESSIBLE_DESCRIPTION));
  load_time_data.SetString("page_switcher_change_title",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_PAGE_SWITCHER_CHANGE_TITLE));
  load_time_data.SetString("page_switcher_same_title",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_PAGE_SWITCHER_SAME_TITLE));
  load_time_data.SetString("appsPromoTitle",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_PAGE_APPS_PROMO_TITLE));
  load_time_data.SetBoolean("isSwipeTrackingFromScrollEventsEnabled",
                            is_swipe_tracking_from_scroll_events_enabled_);
  if (profile_->IsManaged())
    should_show_apps_page_ = false;

  load_time_data.SetBoolean("showApps", should_show_apps_page_);
  load_time_data.SetBoolean("showWebStoreIcon",
                            !prefs->GetBoolean(prefs::kHideWebStoreIcon));

  bool streamlined_hosted_apps = CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableStreamlinedHostedApps);
  load_time_data.SetBoolean("enableStreamlinedHostedApps",
                            streamlined_hosted_apps);
  if (streamlined_hosted_apps) {
    load_time_data.SetString("applaunchtypetab",
         l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_TAB));
   }
 
#if defined(OS_MACOSX)
  load_time_data.SetBoolean(
      "disableCreateAppShortcut",
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kDisableAppShims));
#endif
 #if defined(OS_CHROMEOS)
   load_time_data.SetString("expandMenu",
       l10n_util::GetStringUTF16(IDS_NEW_TAB_CLOSE_MENU_EXPAND));
#endif

  NewTabPageHandler::GetLocalizedValues(profile_, &load_time_data);
  NTPLoginHandler::GetLocalizedValues(profile_, &load_time_data);

  webui::SetFontAndTextDirection(&load_time_data);

  load_time_data.SetBoolean("anim",
                            gfx::Animation::ShouldRenderRichAnimation());

  ui::ThemeProvider* tp = ThemeServiceFactory::GetForProfile(profile_);
  int alignment = tp->GetDisplayProperty(
      ThemeProperties::NTP_BACKGROUND_ALIGNMENT);
  load_time_data.SetString("themegravity",
      (alignment & ThemeProperties::ALIGN_RIGHT) ? "right" : "");

  if (first_run::IsChromeFirstRun()) {
    NotificationPromo::HandleClosed(NotificationPromo::NTP_NOTIFICATION_PROMO);
  } else {
    NotificationPromo notification_promo;
    notification_promo.InitFromPrefs(NotificationPromo::NTP_NOTIFICATION_PROMO);
    if (notification_promo.CanShow()) {
      load_time_data.SetString("notificationPromoText",
                               notification_promo.promo_text());
      DVLOG(1) << "Notification promo:" << notification_promo.promo_text();
    }

    NotificationPromo bubble_promo;
    bubble_promo.InitFromPrefs(NotificationPromo::NTP_BUBBLE_PROMO);
    if (bubble_promo.CanShow()) {
      load_time_data.SetString("bubblePromoText",
                               bubble_promo.promo_text());
      DVLOG(1) << "Bubble promo:" << bubble_promo.promo_text();
    }
  }

  bool show_other_sessions_menu = should_show_other_devices_menu_ &&
      !CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableNTPOtherSessionsMenu);
  load_time_data.SetBoolean("showOtherSessionsMenu", show_other_sessions_menu);
  load_time_data.SetBoolean("isUserSignedIn",
      !prefs->GetString(prefs::kGoogleServicesUsername).empty());

  base::StringPiece new_tab_html(ResourceBundle::GetSharedInstance().
      GetRawDataResource(IDR_NEW_TAB_4_HTML));
  webui::UseVersion2 version2;
  std::string full_html =
      webui::GetI18nTemplateHtml(new_tab_html, &load_time_data);
  new_tab_html_ = base::RefCountedString::TakeString(&full_html);
}
