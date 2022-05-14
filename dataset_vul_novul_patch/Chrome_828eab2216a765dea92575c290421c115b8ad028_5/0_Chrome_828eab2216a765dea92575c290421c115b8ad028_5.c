void RecordDailyContentLengthHistograms(
    int64 original_length,
    int64 received_length,
     int64 original_length_with_data_reduction_enabled,
     int64 received_length_with_data_reduction_enabled,
     int64 original_length_via_data_reduction_proxy,
    int64 received_length_via_data_reduction_proxy,
    int64 https_length_with_data_reduction_enabled,
    int64 short_bypass_length_with_data_reduction_enabled,
    int64 long_bypass_length_with_data_reduction_enabled,
    int64 unknown_length_with_data_reduction_enabled) {
   if (original_length <= 0 || received_length <= 0)
     return;

  UMA_HISTOGRAM_COUNTS(
      "Net.DailyOriginalContentLength", original_length >> 10);
  UMA_HISTOGRAM_COUNTS(
      "Net.DailyContentLength", received_length >> 10);
  int percent = 0;
  if (original_length > received_length) {
    percent = (100 * (original_length - received_length)) / original_length;
  }
  UMA_HISTOGRAM_PERCENTAGE("Net.DailyContentSavingPercent", percent);

  if (original_length_with_data_reduction_enabled <= 0 ||
      received_length_with_data_reduction_enabled <= 0) {
    return;
  }

  UMA_HISTOGRAM_COUNTS(
      "Net.DailyOriginalContentLength_DataReductionProxyEnabled",
      original_length_with_data_reduction_enabled >> 10);
  UMA_HISTOGRAM_COUNTS(
      "Net.DailyContentLength_DataReductionProxyEnabled",
      received_length_with_data_reduction_enabled >> 10);

  int percent_data_reduction_proxy_enabled = 0;
  if (original_length_with_data_reduction_enabled >
      received_length_with_data_reduction_enabled) {
    percent_data_reduction_proxy_enabled =
        100 * (original_length_with_data_reduction_enabled -
               received_length_with_data_reduction_enabled) /
        original_length_with_data_reduction_enabled;
  }
  UMA_HISTOGRAM_PERCENTAGE(
      "Net.DailyContentSavingPercent_DataReductionProxyEnabled",
      percent_data_reduction_proxy_enabled);

  UMA_HISTOGRAM_PERCENTAGE(
       "Net.DailyContentPercent_DataReductionProxyEnabled",
       (100 * received_length_with_data_reduction_enabled) / received_length);
 
  if (https_length_with_data_reduction_enabled > 0) {
    UMA_HISTOGRAM_COUNTS(
        "Net.DailyContentLength_DataReductionProxyEnabled_Https",
        https_length_with_data_reduction_enabled >> 10);
    UMA_HISTOGRAM_PERCENTAGE(
        "Net.DailyContentPercent_DataReductionProxyEnabled_Https",
        (100 * https_length_with_data_reduction_enabled) / received_length);
  }

  if (short_bypass_length_with_data_reduction_enabled > 0) {
    UMA_HISTOGRAM_COUNTS(
        "Net.DailyContentLength_DataReductionProxyEnabled_ShortBypass",
        short_bypass_length_with_data_reduction_enabled >> 10);
    UMA_HISTOGRAM_PERCENTAGE(
        "Net.DailyContentPercent_DataReductionProxyEnabled_ShortBypass",
        ((100 * short_bypass_length_with_data_reduction_enabled) /
         received_length));
  }

  if (long_bypass_length_with_data_reduction_enabled > 0) {
    UMA_HISTOGRAM_COUNTS(
        "Net.DailyContentLength_DataReductionProxyEnabled_LongBypass",
        long_bypass_length_with_data_reduction_enabled >> 10);
    UMA_HISTOGRAM_PERCENTAGE(
        "Net.DailyContentPercent_DataReductionProxyEnabled_LongBypass",
        ((100 * long_bypass_length_with_data_reduction_enabled) /
         received_length));
  }

  if (unknown_length_with_data_reduction_enabled > 0) {
    UMA_HISTOGRAM_COUNTS(
        "Net.DailyContentLength_DataReductionProxyEnabled_Unknown",
        unknown_length_with_data_reduction_enabled >> 10);
    UMA_HISTOGRAM_PERCENTAGE(
        "Net.DailyContentPercent_DataReductionProxyEnabled_Unknown",
        ((100 * unknown_length_with_data_reduction_enabled) /
         received_length));
  }

   if (original_length_via_data_reduction_proxy <= 0 ||
       received_length_via_data_reduction_proxy <= 0) {
     return;
  }

  UMA_HISTOGRAM_COUNTS(
      "Net.DailyOriginalContentLength_ViaDataReductionProxy",
      original_length_via_data_reduction_proxy >> 10);
  UMA_HISTOGRAM_COUNTS(
      "Net.DailyContentLength_ViaDataReductionProxy",
      received_length_via_data_reduction_proxy >> 10);
  int percent_via_data_reduction_proxy = 0;
  if (original_length_via_data_reduction_proxy >
      received_length_via_data_reduction_proxy) {
    percent_via_data_reduction_proxy =
        100 * (original_length_via_data_reduction_proxy -
               received_length_via_data_reduction_proxy) /
        original_length_via_data_reduction_proxy;
  }
  UMA_HISTOGRAM_PERCENTAGE(
      "Net.DailyContentSavingPercent_ViaDataReductionProxy",
      percent_via_data_reduction_proxy);
  UMA_HISTOGRAM_PERCENTAGE(
      "Net.DailyContentPercent_ViaDataReductionProxy",
      (100 * received_length_via_data_reduction_proxy) / received_length);
}
