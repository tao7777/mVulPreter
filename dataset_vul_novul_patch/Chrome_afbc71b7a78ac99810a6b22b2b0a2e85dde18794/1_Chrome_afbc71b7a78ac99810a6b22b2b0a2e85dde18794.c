void OneClickSigninHelper::DidStopLoading(
    content::RenderViewHost* render_view_host) {
  content::WebContents* contents = web_contents();
  const GURL url = contents->GetURL();
  Profile* profile =
      Profile::FromBrowserContext(contents->GetBrowserContext());
  VLOG(1) << "OneClickSigninHelper::DidStopLoading: url=" << url.spec();

  if (!error_message_.empty() && auto_accept_ == AUTO_ACCEPT_EXPLICIT) {
    VLOG(1) << "OneClickSigninHelper::DidStopLoading: error=" << error_message_;
    RemoveCurrentHistoryItem(contents);
    Browser* browser = chrome::FindBrowserWithWebContents(contents);

    RedirectToNtpOrAppsPage(web_contents(), source_);
    ShowSigninErrorBubble(browser, error_message_);
    CleanTransientState();
    return;
  }

  if (AreWeShowingSignin(url, source_, email_)) {
    if (!showing_signin_) {
      if (source_ == SyncPromoUI::SOURCE_UNKNOWN)
        LogOneClickHistogramValue(one_click_signin::HISTOGRAM_SHOWN);
      else
        LogHistogramValue(source_, one_click_signin::HISTOGRAM_SHOWN);
    }
    showing_signin_ = true;
  }

  GURL::Replacements replacements;
  replacements.ClearQuery();
  const bool continue_url_match = (
      continue_url_.is_valid() &&
      url.ReplaceComponents(replacements) ==
        continue_url_.ReplaceComponents(replacements));

  if (continue_url_match)
    RemoveCurrentHistoryItem(contents);

  if (email_.empty()) {
    VLOG(1) << "OneClickSigninHelper::DidStopLoading: nothing to do";
    if (continue_url_match && auto_accept_ == AUTO_ACCEPT_EXPLICIT)
      RedirectToSignin();
    std::string unused_value;
    if (net::GetValueForKeyInQuery(url, "ntp", &unused_value)) {
      SyncPromoUI::SetUserSkippedSyncPromo(profile);
      RedirectToNtpOrAppsPage(web_contents(), source_);
    }

    if (!continue_url_match && !IsValidGaiaSigninRedirectOrResponseURL(url) &&
        ++untrusted_navigations_since_signin_visit_ > kMaxNavigationsSince) {
      CleanTransientState();
    }

    return;
  }

  bool force_same_tab_navigation = false;

  if (!continue_url_match && IsValidGaiaSigninRedirectOrResponseURL(url))
    return;

  if (auto_accept_ == AUTO_ACCEPT_EXPLICIT) {
    DCHECK(source_ != SyncPromoUI::SOURCE_UNKNOWN);
    if (!continue_url_match) {
      VLOG(1) << "OneClickSigninHelper::DidStopLoading: invalid url='"
              << url.spec()
              << "' expected continue url=" << continue_url_;
      CleanTransientState();
      return;
    }

    SyncPromoUI::Source source =
        SyncPromoUI::GetSourceForSyncPromoURL(url);
    if (source != source_) {
      original_source_ = source_;
      source_ = source;
      force_same_tab_navigation = source == SyncPromoUI::SOURCE_SETTINGS;
      switched_to_advanced_ = source == SyncPromoUI::SOURCE_SETTINGS;
    }
  }

  Browser* browser = chrome::FindBrowserWithWebContents(contents);

  VLOG(1) << "OneClickSigninHelper::DidStopLoading: signin is go."
          << " auto_accept=" << auto_accept_
          << " source=" << source_;

  switch (auto_accept_) {
    case AUTO_ACCEPT_NONE:
      if (showing_signin_)
        LogOneClickHistogramValue(one_click_signin::HISTOGRAM_DISMISSED);
      break;
    case AUTO_ACCEPT_ACCEPTED:
      LogOneClickHistogramValue(one_click_signin::HISTOGRAM_ACCEPTED);
      LogOneClickHistogramValue(one_click_signin::HISTOGRAM_WITH_DEFAULTS);
      SigninManager::DisableOneClickSignIn(profile);
      StartSync(StartSyncArgs(profile, browser, auto_accept_,
                              session_index_, email_, password_,
                              false /* force_same_tab_navigation */,
                              true /* confirmation_required */, source_),
                OneClickSigninSyncStarter::SYNC_WITH_DEFAULT_SETTINGS);
      break;
    case AUTO_ACCEPT_CONFIGURE:
       LogOneClickHistogramValue(one_click_signin::HISTOGRAM_ACCEPTED);
       LogOneClickHistogramValue(one_click_signin::HISTOGRAM_WITH_ADVANCED);
       SigninManager::DisableOneClickSignIn(profile);
       StartSync(
           StartSyncArgs(profile, browser, auto_accept_, session_index_, email_,
                         password_, false /* force_same_tab_navigation */,
                        false /* confirmation_required */, source_),
           OneClickSigninSyncStarter::CONFIGURE_SYNC_FIRST);
       break;
     case AUTO_ACCEPT_EXPLICIT: {
      if (switched_to_advanced_) {
        LogHistogramValue(original_source_,
                          one_click_signin::HISTOGRAM_WITH_ADVANCED);
        LogHistogramValue(original_source_,
                          one_click_signin::HISTOGRAM_ACCEPTED);
      } else {
        LogHistogramValue(source_, one_click_signin::HISTOGRAM_ACCEPTED);
        LogHistogramValue(source_, one_click_signin::HISTOGRAM_WITH_DEFAULTS);
      }
      OneClickSigninSyncStarter::StartSyncMode start_mode =
          source_ == SyncPromoUI::SOURCE_SETTINGS ?
              OneClickSigninSyncStarter::CONFIGURE_SYNC_FIRST :
              OneClickSigninSyncStarter::SYNC_WITH_DEFAULT_SETTINGS;

      std::string last_email =
          profile->GetPrefs()->GetString(prefs::kGoogleServicesLastUsername);

      if (!last_email.empty() && !gaia::AreEmailsSame(last_email, email_)) {

        ConfirmEmailDialogDelegate::AskForConfirmation(
            contents,
            last_email,
            email_,
            base::Bind(
                &StartExplicitSync,
                StartSyncArgs(profile, browser, auto_accept_,
                              session_index_, email_, password_,
                              force_same_tab_navigation,
                              false /* confirmation_required */, source_),
                contents,
                start_mode));
      } else {
        StartSync(
            StartSyncArgs(profile, browser, auto_accept_, session_index_,
                          email_, password_, force_same_tab_navigation,
                          untrusted_confirmation_required_, source_),
            start_mode);

        RedirectToNtpOrAppsPageIfNecessary(web_contents(), source_);
      }

      if (source_ == SyncPromoUI::SOURCE_SETTINGS &&
          SyncPromoUI::GetSourceForSyncPromoURL(continue_url_) ==
          SyncPromoUI::SOURCE_WEBSTORE_INSTALL) {
        redirect_url_ = continue_url_;
        ProfileSyncService* sync_service =
          ProfileSyncServiceFactory::GetForProfile(profile);
        if (sync_service)
          sync_service->AddObserver(this);
      }
      break;
    }
    case AUTO_ACCEPT_REJECTED_FOR_PROFILE:
      AddEmailToOneClickRejectedList(profile, email_);
      LogOneClickHistogramValue(one_click_signin::HISTOGRAM_REJECTED);
      break;
    default:
      NOTREACHED() << "Invalid auto_accept=" << auto_accept_;
      break;
  }

  CleanTransientState();
}
