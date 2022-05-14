 const char* GetSRTDownloadURL() {
  if (base::FieldTrialList::FindFullName(kSRTPromptTrial) == kSRTCanaryGroup)
     return kCanarySRTDownloadURL;
   return kMainSRTDownloadURL;
 }
