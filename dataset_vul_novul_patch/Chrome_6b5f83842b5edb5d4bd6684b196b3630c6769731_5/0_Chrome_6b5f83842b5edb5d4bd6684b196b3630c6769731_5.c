void NTPResourceCache::CreateNewTabHTML() {
  DictionaryValue localized_strings;
  localized_strings.SetString("bookmarkbarattached",
      profile_->GetPrefs()->GetBoolean(prefs::kShowBookmarkBar) ?
      "true" : "false");
  localized_strings.SetString("hasattribution",
      ThemeServiceFactory::GetForProfile(profile_)->HasCustomImage(
          IDR_THEME_NTP_ATTRIBUTION) ?
      "true" : "false");
  localized_strings.SetString("title",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_TITLE));
  localized_strings.SetString("mostvisited",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_MOST_VISITED));
  localized_strings.SetString("restoreThumbnailsShort",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RESTORE_THUMBNAILS_SHORT_LINK));
  localized_strings.SetString("recentlyclosed",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RECENTLY_CLOSED));
  localized_strings.SetString("closedwindowsingle",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RECENTLY_CLOSED_WINDOW_SINGLE));
  localized_strings.SetString("closedwindowmultiple",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_RECENTLY_CLOSED_WINDOW_MULTIPLE));
  localized_strings.SetString("attributionintro",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_ATTRIBUTION_INTRO));
  localized_strings.SetString("thumbnailremovednotification",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_THUMBNAIL_REMOVED_NOTIFICATION));
  localized_strings.SetString("undothumbnailremove",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_UNDO_THUMBNAIL_REMOVE));
   localized_strings.SetString("removethumbnailtooltip",
       l10n_util::GetStringUTF16(IDS_NEW_TAB_REMOVE_THUMBNAIL_TOOLTIP));
   localized_strings.SetString("appuninstall",
      l10n_util::GetStringUTF16(IDS_EXTENSIONS_UNINSTALL));
   localized_strings.SetString("appoptions",
       l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_OPTIONS));
   localized_strings.SetString("appdisablenotifications",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_DISABLE_NOTIFICATIONS));
  localized_strings.SetString("appcreateshortcut",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_APP_CREATE_SHORTCUT));
  localized_strings.SetString("appDefaultPageName",
      l10n_util::GetStringUTF16(IDS_APP_DEFAULT_PAGE_NAME));
  localized_strings.SetString("applaunchtypepinned",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_PINNED));
  localized_strings.SetString("applaunchtyperegular",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_REGULAR));
  localized_strings.SetString("applaunchtypewindow",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_WINDOW));
  localized_strings.SetString("applaunchtypefullscreen",
      l10n_util::GetStringUTF16(IDS_APP_CONTEXT_MENU_OPEN_FULLSCREEN));
  localized_strings.SetString("syncpromotext",
      l10n_util::GetStringUTF16(IDS_SYNC_START_SYNC_BUTTON_LABEL));
  localized_strings.SetString("syncLinkText",
      l10n_util::GetStringUTF16(IDS_SYNC_ADVANCED_OPTIONS));
#if defined(OS_CHROMEOS)
  localized_strings.SetString("expandMenu",
      l10n_util::GetStringUTF16(IDS_NEW_TAB_CLOSE_MENU_EXPAND));
#endif

  NewTabPageHandler::GetLocalizedValues(profile_, &localized_strings);
  NTPLoginHandler::GetLocalizedValues(profile_, &localized_strings);

  if (profile_->GetProfileSyncService())
    localized_strings.SetString("syncispresent", "true");
  else
    localized_strings.SetString("syncispresent", "false");

  ChromeURLDataManager::DataSource::SetFontAndTextDirection(&localized_strings);

  std::string anim =
      ui::Animation::ShouldRenderRichAnimation() ? "true" : "false";
  localized_strings.SetString("anim", anim);

  int alignment;
  ui::ThemeProvider* tp = ThemeServiceFactory::GetForProfile(profile_);
  tp->GetDisplayProperty(ThemeService::NTP_BACKGROUND_ALIGNMENT, &alignment);
  localized_strings.SetString("themegravity",
      (alignment & ThemeService::ALIGN_RIGHT) ? "right" : "");

  if (profile_->GetPrefs()->FindPreference(prefs::kNTPCustomLogoStart) &&
      profile_->GetPrefs()->FindPreference(prefs::kNTPCustomLogoEnd)) {
    localized_strings.SetString("customlogo",
        InDateRange(profile_->GetPrefs()->GetDouble(prefs::kNTPCustomLogoStart),
                    profile_->GetPrefs()->GetDouble(prefs::kNTPCustomLogoEnd)) ?
        "true" : "false");
  } else {
    localized_strings.SetString("customlogo", "false");
  }

  if (PromoResourceService::CanShowNotificationPromo(profile_)) {
    localized_strings.SetString("serverpromo",
        profile_->GetPrefs()->GetString(prefs::kNTPPromoLine));
  }

  std::string full_html;
  base::StringPiece new_tab_html(ResourceBundle::GetSharedInstance().
      GetRawDataResource(IDR_NEW_TAB_4_HTML));
  full_html = jstemplate_builder::GetI18nTemplateHtml(new_tab_html,
                                                      &localized_strings);
  new_tab_html_ = base::RefCountedString::TakeString(&full_html);
}
