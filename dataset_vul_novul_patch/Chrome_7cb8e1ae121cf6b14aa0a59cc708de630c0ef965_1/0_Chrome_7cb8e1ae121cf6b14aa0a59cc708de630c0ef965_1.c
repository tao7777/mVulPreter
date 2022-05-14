 std::string MasterPreferences::GetVariationsSeed() const {
  return ExtractPrefString(chrome_variations::prefs::kVariationsSeed);
 }
