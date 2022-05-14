 void UpdateContentLengthPrefs(
    int received_content_length,
    int original_content_length,
    bool with_data_reduction_proxy_enabled,
    DataReductionRequestType data_reduction_type,
     PrefService* prefs) {
   int64 total_received = prefs->GetInt64(prefs::kHttpReceivedContentLength);
   int64 total_original = prefs->GetInt64(prefs::kHttpOriginalContentLength);
  total_received += received_content_length;
  total_original += original_content_length;
  prefs->SetInt64(prefs::kHttpReceivedContentLength, total_received);
  prefs->SetInt64(prefs::kHttpOriginalContentLength, total_original);

#if defined(OS_ANDROID) || defined(OS_IOS)
  UpdateContentLengthPrefsForDataReductionProxy(
       received_content_length,
       original_content_length,
       with_data_reduction_proxy_enabled,
      data_reduction_type,
       base::Time::Now(),
       prefs);
 #endif  // defined(OS_ANDROID) || defined(OS_IOS)

}
