 std::string MasterPreferences::GetCompressedVariationsSeed() const {
  return ExtractPrefString(chrome_variations::prefs::kVariationsCompressedSeed);
 }
