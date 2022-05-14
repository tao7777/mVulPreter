void Preferences::Init(PrefService* prefs) {
  tap_to_click_enabled_.Init(prefs::kTapToClickEnabled, prefs, this);
  accessibility_enabled_.Init(prefs::kAccessibilityEnabled, prefs, this);
  vert_edge_scroll_enabled_.Init(prefs::kVertEdgeScrollEnabled, prefs, this);
  speed_factor_.Init(prefs::kTouchpadSpeedFactor, prefs, this);
  sensitivity_.Init(prefs::kTouchpadSensitivity, prefs, this);
  language_hotkey_next_engine_in_menu_.Init(
      prefs::kLanguageHotkeyNextEngineInMenu, prefs, this);
  language_hotkey_previous_engine_.Init(
      prefs::kLanguageHotkeyPreviousEngine, prefs, this);
  language_preload_engines_.Init(prefs::kLanguagePreloadEngines, prefs, this);
  for (size_t i = 0; i < kNumChewingBooleanPrefs; ++i) {
    language_chewing_boolean_prefs_[i].Init(
        kChewingBooleanPrefs[i].pref_name, prefs, this);
  }
  for (size_t i = 0; i < kNumChewingMultipleChoicePrefs; ++i) {
    language_chewing_multiple_choice_prefs_[i].Init(
        kChewingMultipleChoicePrefs[i].pref_name, prefs, this);
  }
  language_chewing_hsu_sel_key_type_.Init(
      kChewingHsuSelKeyType.pref_name, prefs, this);
  for (size_t i = 0; i < kNumChewingIntegerPrefs; ++i) {
    language_chewing_integer_prefs_[i].Init(
         kChewingIntegerPrefs[i].pref_name, prefs, this);
   }
   language_hangul_keyboard_.Init(prefs::kLanguageHangulKeyboard, prefs, this);
  language_hangul_hanja_keys_.Init(
      prefs::kLanguageHangulHanjaKeys, prefs, this);
   for (size_t i = 0; i < kNumPinyinBooleanPrefs; ++i) {
     language_pinyin_boolean_prefs_[i].Init(
         kPinyinBooleanPrefs[i].pref_name, prefs, this);
  }
  for (size_t i = 0; i < kNumPinyinIntegerPrefs; ++i) {
    language_pinyin_int_prefs_[i].Init(
        kPinyinIntegerPrefs[i].pref_name, prefs, this);
  }
  language_pinyin_double_pinyin_schema_.Init(
      kPinyinDoublePinyinSchema.pref_name, prefs, this);
  for (size_t i = 0; i < kNumMozcBooleanPrefs; ++i) {
    language_mozc_boolean_prefs_[i].Init(
        kMozcBooleanPrefs[i].pref_name, prefs, this);
  }
  for (size_t i = 0; i < kNumMozcMultipleChoicePrefs; ++i) {
    language_mozc_multiple_choice_prefs_[i].Init(
        kMozcMultipleChoicePrefs[i].pref_name, prefs, this);
  }
  for (size_t i = 0; i < kNumMozcIntegerPrefs; ++i) {
    language_mozc_integer_prefs_[i].Init(
        kMozcIntegerPrefs[i].pref_name, prefs, this);
  }

  std::string locale(g_browser_process->GetApplicationLocale());
  if (locale != kFallbackInputMethodLocale &&
      !prefs->HasPrefPath(prefs::kLanguagePreloadEngines)) {
    std::string preload_engines(language_preload_engines_.GetValue());
    std::vector<std::string> input_method_ids;
    input_method::GetInputMethodIdsFromLanguageCode(
        locale, input_method::kAllInputMethods, &input_method_ids);
    if (!input_method_ids.empty()) {
      if (!preload_engines.empty())
        preload_engines += ',';
      preload_engines += JoinString(input_method_ids, ',');
    }
    language_preload_engines_.SetValue(preload_engines);
  }

  NotifyPrefChanged(NULL);
}
