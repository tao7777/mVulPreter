 void CachingPermutedEntropyProvider::ClearCache(PrefService* local_state) {
  local_state->ClearPref(prefs::kVariationsPermutedEntropyCache);
 }
