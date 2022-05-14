void CachingPermutedEntropyProvider::UpdateLocalState() const {
  std::string serialized;
  cache_.SerializeToString(&serialized);
 
   std::string base64_encoded;
   base::Base64Encode(serialized, &base64_encoded);
  local_state_->SetString(prefs::kVariationsPermutedEntropyCache,
                          base64_encoded);
 }
