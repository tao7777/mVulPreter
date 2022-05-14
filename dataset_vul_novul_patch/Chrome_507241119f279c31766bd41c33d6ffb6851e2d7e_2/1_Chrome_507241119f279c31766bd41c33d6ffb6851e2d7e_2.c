bool CheckClientDownloadRequest::ShouldUploadForDlpScan() {
  if (!base::FeatureList::IsEnabled(kDeepScanningOfDownloads))
    return false;

  int check_content_compliance = g_browser_process->local_state()->GetInteger(
      prefs::kCheckContentCompliance);
  if (check_content_compliance !=
          CheckContentComplianceValues::CHECK_DOWNLOADS &&
      check_content_compliance !=
           CheckContentComplianceValues::CHECK_UPLOADS_AND_DOWNLOADS)
     return false;
 
  if (policy::BrowserDMTokenStorage::Get()->RetrieveDMToken().empty())
     return false;
 
   const base::ListValue* domains = g_browser_process->local_state()->GetList(
      prefs::kURLsToCheckComplianceOfDownloadedContent);
  url_matcher::URLMatcher matcher;
  policy::url_util::AddAllowFilters(&matcher, domains);
  return !matcher.MatchURL(item_->GetURL()).empty();
}
