  void VerifyDailyContentLengthPrefLists(
       const int64* original_values, size_t original_count,
       const int64* received_values, size_t received_count,
       const int64* original_with_data_reduction_proxy_enabled_values,
      size_t original_with_data_reduction_proxy_enabled_count,
      const int64* received_with_data_reduction_proxy_enabled_values,
      size_t received_with_data_reduction_proxy_count,
      const int64* original_via_data_reduction_proxy_values,
      size_t original_via_data_reduction_proxy_count,
      const int64* received_via_data_reduction_proxy_values,
      size_t received_via_data_reduction_proxy_count) {
    VerifyPrefList(prefs::kDailyHttpOriginalContentLength,
                   original_values, original_count);
    VerifyPrefList(prefs::kDailyHttpReceivedContentLength,
                   received_values, received_count);
    VerifyPrefList(
        prefs::kDailyOriginalContentLengthWithDataReductionProxyEnabled,
        original_with_data_reduction_proxy_enabled_values,
        original_with_data_reduction_proxy_enabled_count);
    VerifyPrefList(
        prefs::kDailyContentLengthWithDataReductionProxyEnabled,
        received_with_data_reduction_proxy_enabled_values,
        received_with_data_reduction_proxy_count);
    VerifyPrefList(
        prefs::kDailyOriginalContentLengthViaDataReductionProxy,
        original_via_data_reduction_proxy_values,
        original_via_data_reduction_proxy_count);
    VerifyPrefList(
        prefs::kDailyContentLengthViaDataReductionProxy,
        received_via_data_reduction_proxy_values,
         received_via_data_reduction_proxy_count);
   }
