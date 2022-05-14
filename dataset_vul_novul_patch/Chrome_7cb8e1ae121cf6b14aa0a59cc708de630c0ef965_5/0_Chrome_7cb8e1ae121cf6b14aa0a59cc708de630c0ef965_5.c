 void CachingPermutedEntropyProvider::RegisterPrefs(
     PrefRegistrySimple* registry) {
  registry->RegisterStringPref(
      chrome_variations::prefs::kVariationsPermutedEntropyCache, std::string());
 }
