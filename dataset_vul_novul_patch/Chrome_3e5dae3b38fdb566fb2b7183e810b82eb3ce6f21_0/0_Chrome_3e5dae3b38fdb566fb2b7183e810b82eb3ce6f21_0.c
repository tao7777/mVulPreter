 const char* GetSRTDownloadURL() {
  if (base::StartsWith(base::FieldTrialList::FindFullName(kSRTPromptTrial),
                       kSRTCanaryGroup, base::CompareCase::SENSITIVE)) {
     return kCanarySRTDownloadURL;
  }
   return kMainSRTDownloadURL;
 }
