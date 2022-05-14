void SafeBrowsingBlockingPage::CommandReceived(const std::string& cmd) {
  std::string command(cmd);  // Make a local copy so we can modify it.
  if (command.length() > 1 && command[0] == '"') {
    command = command.substr(1, command.length() - 2);
  }
  RecordUserReactionTime(command);
  if (command == kDoReportCommand) {
    SetReportingPreference(true);
    return;
  }

  if (command == kDontReportCommand) {
    SetReportingPreference(false);
    return;
  }

  if (command == kLearnMoreCommand) {
    GURL url;
    SBThreatType threat_type = unsafe_resources_[0].threat_type;
    if (threat_type == SB_THREAT_TYPE_URL_MALWARE) {
      url = google_util::AppendGoogleLocaleParam(GURL(kLearnMoreMalwareUrl));
    } else if (threat_type == SB_THREAT_TYPE_URL_PHISHING ||
               threat_type == SB_THREAT_TYPE_CLIENT_SIDE_PHISHING_URL) {
      url = google_util::AppendGoogleLocaleParam(GURL(kLearnMorePhishingUrl));
    } else {
      NOTREACHED();
    }

    OpenURLParams params(
        url, Referrer(), CURRENT_TAB, content::PAGE_TRANSITION_LINK, false);
    web_contents_->OpenURL(params);
    return;
  }

  if (command == kLearnMoreCommandV2) {
    GURL url;
    SBThreatType threat_type = unsafe_resources_[0].threat_type;
    if (threat_type == SB_THREAT_TYPE_URL_MALWARE) {
      url = google_util::AppendGoogleLocaleParam(GURL(kLearnMoreMalwareUrlV2));
    } else if (threat_type == SB_THREAT_TYPE_URL_PHISHING ||
               threat_type == SB_THREAT_TYPE_CLIENT_SIDE_PHISHING_URL) {
      url = google_util::AppendGoogleLocaleParam(GURL(kLearnMorePhishingUrlV2));
    } else {
      NOTREACHED();
    }

    OpenURLParams params(
        url, Referrer(), CURRENT_TAB, content::PAGE_TRANSITION_LINK, false);
    web_contents_->OpenURL(params);
    return;
  }

  if (command == kShowPrivacyCommand) {
    GURL url(l10n_util::GetStringUTF8(IDS_SAFE_BROWSING_PRIVACY_POLICY_URL));
    OpenURLParams params(
        url, Referrer(), CURRENT_TAB, content::PAGE_TRANSITION_LINK, false);
    web_contents_->OpenURL(params);
    return;
  }

  bool proceed_blocked = false;
  if (command == kProceedCommand) {
    if (IsPrefEnabled(prefs::kSafeBrowsingProceedAnywayDisabled)) {
      proceed_blocked = true;
    } else {
      interstitial_page_->Proceed();
      return;
    }
  }

  if (command == kTakeMeBackCommand || proceed_blocked) {
    if (is_main_frame_load_blocked_) {
      interstitial_page_->DontProceed();
      return;
    }

    if (web_contents_->GetController().CanGoBack()) {
      web_contents_->GetController().GoBack();
    } else {
      web_contents_->GetController().LoadURL(
          GURL(chrome::kChromeUINewTabURL),
          content::Referrer(),
          content::PAGE_TRANSITION_AUTO_TOPLEVEL,
          std::string());
    }
    return;
  }
 
  size_t element_index = 0;
   size_t colon_index = command.find(':');
   if (colon_index != std::string::npos) {
     DCHECK(colon_index < command.size() - 1);
    int result_int = 0;
     bool result = base::StringToInt(base::StringPiece(command.begin() +
                                                       colon_index + 1,
                                                       command.end()),
                                    &result_int);
     command = command.substr(0, colon_index);
    if (result)
      element_index = static_cast<size_t>(result_int);
   }
 
  if (element_index >= unsafe_resources_.size()) {
     NOTREACHED();
     return;
   }

  std::string bad_url_spec = unsafe_resources_[element_index].url.spec();
  if (command == kReportErrorCommand) {
    SBThreatType threat_type = unsafe_resources_[element_index].threat_type;
    DCHECK(threat_type == SB_THREAT_TYPE_URL_PHISHING ||
           threat_type == SB_THREAT_TYPE_CLIENT_SIDE_PHISHING_URL);
    GURL report_url =
        safe_browsing_util::GeneratePhishingReportUrl(
            kSbReportPhishingErrorUrl,
            bad_url_spec,
            threat_type == SB_THREAT_TYPE_CLIENT_SIDE_PHISHING_URL);
    OpenURLParams params(
        report_url, Referrer(), CURRENT_TAB, content::PAGE_TRANSITION_LINK,
        false);
    web_contents_->OpenURL(params);
    return;
  }

  if (command == kShowDiagnosticCommand) {
    std::string diagnostic =
        base::StringPrintf(kSbDiagnosticUrl,
            net::EscapeQueryParamValue(bad_url_spec, true).c_str());
    GURL diagnostic_url(diagnostic);
    diagnostic_url = google_util::AppendGoogleLocaleParam(diagnostic_url);
    DCHECK(unsafe_resources_[element_index].threat_type ==
           SB_THREAT_TYPE_URL_MALWARE);
    OpenURLParams params(
        diagnostic_url, Referrer(), CURRENT_TAB, content::PAGE_TRANSITION_LINK,
        false);
    web_contents_->OpenURL(params);
    return;
  }

  if (command == kExpandedSeeMore) {
    return;
  }

  NOTREACHED() << "Unexpected command: " << command;
}
