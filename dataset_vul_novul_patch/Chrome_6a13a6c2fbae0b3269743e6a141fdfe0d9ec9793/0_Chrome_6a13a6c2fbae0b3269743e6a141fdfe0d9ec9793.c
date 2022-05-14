void SafeBrowsingBlockingPage::CommandReceived(const std::string& cmd) {
  std::string command(cmd);  // Make a local copy so we can modify it.
  if (command.length() > 1 && command[0] == '"') {
    command = command.substr(1, command.length() - 2);
  }

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
    SafeBrowsingService::UrlCheckResult threat_type =
        unsafe_resources_[0].threat_type;
    if (threat_type == SafeBrowsingService::URL_MALWARE) {
      url = google_util::AppendGoogleLocaleParam(GURL(kLearnMoreMalwareUrl));
    } else if (threat_type == SafeBrowsingService::URL_PHISHING ||
               threat_type == SafeBrowsingService::CLIENT_SIDE_PHISHING_URL) {
      url = google_util::AppendGoogleLocaleParam(GURL(kLearnMorePhishingUrl));
    } else {
      NOTREACHED();
    }
    tab()->OpenURL(url, GURL(), CURRENT_TAB, content::PAGE_TRANSITION_LINK);
    return;
  }

  if (command == kShowPrivacyCommand) {
    GURL url(kSbPrivacyPolicyUrl);
    tab()->OpenURL(url, GURL(), CURRENT_TAB, content::PAGE_TRANSITION_LINK);
    return;
  }

  if (command == kProceedCommand) {
    Proceed();
    return;
   }
 
   if (command == kTakeMeBackCommand) {
    if (is_main_frame_load_blocked_) {
      // If the load is blocked, we want to close the interstitial and discard
      // the pending entry.
      DontProceed();
      // We are deleted after this.
      return;
    }

    // Otherwise the offending entry has committed, and we need to go back or
    // to a safe page.  We will close the interstitial when that page commits.
    if (tab()->controller().CanGoBack()) {
      tab()->controller().GoBack();
    } else {
      tab()->controller().LoadURL(GURL(chrome::kChromeUINewTabURL),
                                  content::Referrer(),
                                  content::PAGE_TRANSITION_START_PAGE,
                                  std::string());
    }
     return;
   }
 
  int element_index = 0;
  size_t colon_index = command.find(':');
  if (colon_index != std::string::npos) {
    DCHECK(colon_index < command.size() - 1);
    bool result = base::StringToInt(base::StringPiece(command.begin() +
                                                      colon_index + 1,
                                                      command.end()),
                                    &element_index);
    command = command.substr(0, colon_index);
    DCHECK(result);
  }

  if (element_index >= static_cast<int>(unsafe_resources_.size())) {
    NOTREACHED();
    return;
  }

  std::string bad_url_spec = unsafe_resources_[element_index].url.spec();
  if (command == kReportErrorCommand) {
    SafeBrowsingService::UrlCheckResult threat_type =
        unsafe_resources_[element_index].threat_type;
    DCHECK(threat_type == SafeBrowsingService::URL_PHISHING ||
           threat_type == SafeBrowsingService::CLIENT_SIDE_PHISHING_URL);
    GURL report_url =
        safe_browsing_util::GeneratePhishingReportUrl(
            kSbReportPhishingErrorUrl,
            bad_url_spec,
            threat_type == SafeBrowsingService::CLIENT_SIDE_PHISHING_URL);
    tab()->OpenURL(
        report_url, GURL(), CURRENT_TAB, content::PAGE_TRANSITION_LINK);
    return;
  }

  if (command == kShowDiagnosticCommand) {
    std::string diagnostic =
        base::StringPrintf(kSbDiagnosticUrl,
            net::EscapeQueryParamValue(bad_url_spec, true).c_str());
    GURL diagnostic_url(diagnostic);
    diagnostic_url = google_util::AppendGoogleLocaleParam(diagnostic_url);
    DCHECK(unsafe_resources_[element_index].threat_type ==
           SafeBrowsingService::URL_MALWARE);
    tab()->OpenURL(
        diagnostic_url, GURL(), CURRENT_TAB, content::PAGE_TRANSITION_LINK);
    return;
  }

  NOTREACHED() << "Unexpected command: " << command;
}
