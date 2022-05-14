 std::string MasterPreferences::GetVariationsSeedSignature() const {
  return ExtractPrefString(chrome_variations::prefs::kVariationsSeedSignature);
 }
