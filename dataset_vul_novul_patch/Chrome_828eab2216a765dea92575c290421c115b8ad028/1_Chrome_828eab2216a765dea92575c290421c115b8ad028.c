void IOThread::RegisterPrefs(PrefRegistrySimple* registry) {
  registry->RegisterStringPref(prefs::kAuthSchemes,
                               "basic,digest,ntlm,negotiate,"
                               "spdyproxy");
  registry->RegisterBooleanPref(prefs::kDisableAuthNegotiateCnameLookup, false);
  registry->RegisterBooleanPref(prefs::kEnableAuthNegotiatePort, false);
  registry->RegisterStringPref(prefs::kAuthServerWhitelist, std::string());
  registry->RegisterStringPref(prefs::kAuthNegotiateDelegateWhitelist,
                               std::string());
  registry->RegisterStringPref(prefs::kGSSAPILibraryName, std::string());
  registry->RegisterStringPref(prefs::kSpdyProxyAuthOrigin, std::string());
  registry->RegisterBooleanPref(prefs::kEnableReferrers, true);
  registry->RegisterInt64Pref(prefs::kHttpReceivedContentLength, 0);
  registry->RegisterInt64Pref(prefs::kHttpOriginalContentLength, 0);
#if defined(OS_ANDROID) || defined(OS_IOS)
  registry->RegisterListPref(prefs::kDailyHttpOriginalContentLength);
  registry->RegisterListPref(prefs::kDailyHttpReceivedContentLength);
  registry->RegisterListPref(
       prefs::kDailyOriginalContentLengthWithDataReductionProxyEnabled);
   registry->RegisterListPref(
       prefs::kDailyContentLengthWithDataReductionProxyEnabled);
   registry->RegisterListPref(
       prefs::kDailyOriginalContentLengthViaDataReductionProxy);
   registry->RegisterListPref(
      prefs::kDailyContentLengthViaDataReductionProxy);
  registry->RegisterInt64Pref(prefs::kDailyHttpContentLengthLastUpdateDate, 0L);
#endif
  registry->RegisterBooleanPref(prefs::kBuiltInDnsClientEnabled, true);
}
