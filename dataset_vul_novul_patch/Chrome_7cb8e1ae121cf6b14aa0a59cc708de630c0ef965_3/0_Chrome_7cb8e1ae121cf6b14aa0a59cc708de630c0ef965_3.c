 void CachingPermutedEntropyProvider::ClearCache(PrefService* local_state) {
  local_state->ClearPref(
      chrome_variations::prefs::kVariationsPermutedEntropyCache);
 }
