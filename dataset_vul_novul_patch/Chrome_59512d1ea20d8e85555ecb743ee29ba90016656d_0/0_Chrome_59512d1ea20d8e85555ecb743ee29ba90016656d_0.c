void ContentSuggestionsNotifierService::RegisterProfilePrefs(
     user_prefs::PrefRegistrySyncable* registry) {
   registry->RegisterIntegerPref(
       prefs::kContentSuggestionsConsecutiveIgnoredPrefName, 0);
  registry->RegisterIntegerPref(prefs::kContentSuggestionsNotificationsSentDay,
                                0);
  registry->RegisterIntegerPref(
      prefs::kContentSuggestionsNotificationsSentCount, 0);
 
   registry->RegisterStringPref(kNotificationIDWithinCategory, std::string());
}
