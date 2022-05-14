void Preferences::NotifyPrefChanged(const std::wstring* pref_name) {
  if (!pref_name || *pref_name == prefs::kTapToClickEnabled) {
    CrosLibrary::Get()->GetSynapticsLibrary()->SetBoolParameter(
        PARAM_BOOL_TAP_TO_CLICK,
        tap_to_click_enabled_.GetValue());
  }
  if (!pref_name || *pref_name == prefs::kVertEdgeScrollEnabled) {
    CrosLibrary::Get()->GetSynapticsLibrary()->SetBoolParameter(
        PARAM_BOOL_VERTICAL_EDGE_SCROLLING,
        vert_edge_scroll_enabled_.GetValue());
  }
  if (!pref_name || *pref_name == prefs::kTouchpadSpeedFactor) {
    CrosLibrary::Get()->GetSynapticsLibrary()->SetRangeParameter(
        PARAM_RANGE_SPEED_SENSITIVITY,
        speed_factor_.GetValue());
  }
  if (!pref_name || *pref_name == prefs::kTouchpadSensitivity) {
    CrosLibrary::Get()->GetSynapticsLibrary()->SetRangeParameter(
          PARAM_RANGE_TOUCH_SENSITIVITY,
          sensitivity_.GetValue());
  }


  if (!pref_name || *pref_name == prefs::kLanguageHotkeyNextEngineInMenu) {
    SetLanguageConfigStringListAsCSV(
        kHotKeySectionName,
        kNextEngineInMenuConfigName,
        language_hotkey_next_engine_in_menu_.GetValue());
  }
  if (!pref_name || *pref_name == prefs::kLanguageHotkeyPreviousEngine) {
    SetLanguageConfigStringListAsCSV(
        kHotKeySectionName,
        kPreviousEngineConfigName,
        language_hotkey_previous_engine_.GetValue());
  }
  if (!pref_name || *pref_name == prefs::kLanguagePreloadEngines) {
    SetLanguageConfigStringListAsCSV(kGeneralSectionName,
                                     kPreloadEnginesConfigName,
                                     language_preload_engines_.GetValue());
  }
  for (size_t i = 0; i < kNumChewingBooleanPrefs; ++i) {
    if (!pref_name || *pref_name == kChewingBooleanPrefs[i].pref_name) {
      SetLanguageConfigBoolean(kChewingSectionName,
                               kChewingBooleanPrefs[i].ibus_config_name,
                               language_chewing_boolean_prefs_[i].GetValue());
    }
  }
  for (size_t i = 0; i < kNumChewingMultipleChoicePrefs; ++i) {
    if (!pref_name || *pref_name == kChewingMultipleChoicePrefs[i].pref_name) {
      SetLanguageConfigString(
          kChewingSectionName,
          kChewingMultipleChoicePrefs[i].ibus_config_name,
          language_chewing_multiple_choice_prefs_[i].GetValue());
    }
  }
  if (!pref_name || *pref_name == kChewingHsuSelKeyType.pref_name) {
    SetLanguageConfigInteger(
        kChewingSectionName,
        kChewingHsuSelKeyType.ibus_config_name,
        language_chewing_hsu_sel_key_type_.GetValue());
  }
  for (size_t i = 0; i < kNumChewingIntegerPrefs; ++i) {
    if (!pref_name || *pref_name == kChewingIntegerPrefs[i].pref_name) {
      SetLanguageConfigInteger(kChewingSectionName,
                               kChewingIntegerPrefs[i].ibus_config_name,
                               language_chewing_integer_prefs_[i].GetValue());
    }
  }
  if (!pref_name || *pref_name == prefs::kLanguageHangulKeyboard) {
     SetLanguageConfigString(kHangulSectionName, kHangulKeyboardConfigName,
                             language_hangul_keyboard_.GetValue());
   }
   for (size_t i = 0; i < kNumPinyinBooleanPrefs; ++i) {
     if (!pref_name || *pref_name == kPinyinBooleanPrefs[i].pref_name) {
       SetLanguageConfigBoolean(kPinyinSectionName,
                               kPinyinBooleanPrefs[i].ibus_config_name,
                               language_pinyin_boolean_prefs_[i].GetValue());
    }
  }
  for (size_t i = 0; i < kNumPinyinIntegerPrefs; ++i) {
    if (!pref_name || *pref_name == kPinyinIntegerPrefs[i].pref_name) {
      SetLanguageConfigInteger(kPinyinSectionName,
                               kPinyinIntegerPrefs[i].ibus_config_name,
                               language_pinyin_int_prefs_[i].GetValue());
    }
  }
  if (!pref_name || *pref_name == kPinyinDoublePinyinSchema.pref_name) {
    SetLanguageConfigInteger(
        kPinyinSectionName,
        kPinyinDoublePinyinSchema.ibus_config_name,
        language_pinyin_double_pinyin_schema_.GetValue());
  }
  for (size_t i = 0; i < kNumMozcBooleanPrefs; ++i) {
    if (!pref_name || *pref_name == kMozcBooleanPrefs[i].pref_name) {
      SetLanguageConfigBoolean(kMozcSectionName,
                               kMozcBooleanPrefs[i].ibus_config_name,
                               language_mozc_boolean_prefs_[i].GetValue());
    }
  }
  for (size_t i = 0; i < kNumMozcMultipleChoicePrefs; ++i) {
    if (!pref_name || *pref_name == kMozcMultipleChoicePrefs[i].pref_name) {
      SetLanguageConfigString(
          kMozcSectionName,
          kMozcMultipleChoicePrefs[i].ibus_config_name,
          language_mozc_multiple_choice_prefs_[i].GetValue());
    }
  }
  for (size_t i = 0; i < kNumMozcIntegerPrefs; ++i) {
    if (!pref_name || *pref_name == kMozcIntegerPrefs[i].pref_name) {
      SetLanguageConfigInteger(kMozcSectionName,
                               kMozcIntegerPrefs[i].ibus_config_name,
                               language_mozc_integer_prefs_[i].GetValue());
    }
  }
}
