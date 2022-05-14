 void CachingPermutedEntropyProvider::ReadFromLocalState() const {
  const std::string base64_cache_data =
      local_state_->GetString(prefs::kVariationsPermutedEntropyCache);
   std::string cache_data;
   if (!base::Base64Decode(base64_cache_data, &cache_data) ||
       !cache_.ParseFromString(cache_data)) {
    local_state_->ClearPref(prefs::kVariationsPermutedEntropyCache);
     NOTREACHED();
   }
 }
