void Preferences::RegisterUserPrefs(PrefService* prefs) {
  prefs->RegisterBooleanPref(prefs::kTapToClickEnabled, false);
  prefs->RegisterBooleanPref(prefs::kLabsMediaplayerEnabled, false);
  prefs->RegisterBooleanPref(prefs::kLabsAdvancedFilesystemEnabled, false);
  prefs->RegisterBooleanPref(prefs::kAccessibilityEnabled, false);
  prefs->RegisterBooleanPref(prefs::kVertEdgeScrollEnabled, false);
  prefs->RegisterIntegerPref(prefs::kTouchpadSpeedFactor, 9);
  prefs->RegisterIntegerPref(prefs::kTouchpadSensitivity, 5);
  prefs->RegisterStringPref(prefs::kLanguageCurrentInputMethod, "");
  prefs->RegisterStringPref(prefs::kLanguagePreviousInputMethod, "");
  prefs->RegisterStringPref(prefs::kLanguageHotkeyNextEngineInMenu,
                            kHotkeyNextEngineInMenu);
  prefs->RegisterStringPref(prefs::kLanguageHotkeyPreviousEngine,
                            kHotkeyPreviousEngine);
  prefs->RegisterStringPref(prefs::kLanguagePreloadEngines,
                            kFallbackInputMethodId);  // EN layout
  for (size_t i = 0; i < kNumChewingBooleanPrefs; ++i) {
    prefs->RegisterBooleanPref(kChewingBooleanPrefs[i].pref_name,
                               kChewingBooleanPrefs[i].default_pref_value);
  }
  for (size_t i = 0; i < kNumChewingMultipleChoicePrefs; ++i) {
    prefs->RegisterStringPref(
        kChewingMultipleChoicePrefs[i].pref_name,
        kChewingMultipleChoicePrefs[i].default_pref_value);
  }
  prefs->RegisterIntegerPref(kChewingHsuSelKeyType.pref_name,
                             kChewingHsuSelKeyType.default_pref_value);

  for (size_t i = 0; i < kNumChewingIntegerPrefs; ++i) {
    prefs->RegisterIntegerPref(kChewingIntegerPrefs[i].pref_name,
                               kChewingIntegerPrefs[i].default_pref_value);
  }
   prefs->RegisterStringPref(
       prefs::kLanguageHangulKeyboard,
       kHangulKeyboardNameIDPairs[0].keyboard_id);
  prefs->RegisterStringPref(prefs::kLanguageHangulHanjaKeys, kHangulHanjaKeys);
   for (size_t i = 0; i < kNumPinyinBooleanPrefs; ++i) {
     prefs->RegisterBooleanPref(kPinyinBooleanPrefs[i].pref_name,
                                kPinyinBooleanPrefs[i].default_pref_value);
  }
  for (size_t i = 0; i < kNumPinyinIntegerPrefs; ++i) {
    prefs->RegisterIntegerPref(kPinyinIntegerPrefs[i].pref_name,
                               kPinyinIntegerPrefs[i].default_pref_value);
  }
  prefs->RegisterIntegerPref(kPinyinDoublePinyinSchema.pref_name,
                             kPinyinDoublePinyinSchema.default_pref_value);

  for (size_t i = 0; i < kNumMozcBooleanPrefs; ++i) {
    prefs->RegisterBooleanPref(kMozcBooleanPrefs[i].pref_name,
                               kMozcBooleanPrefs[i].default_pref_value);
  }
  for (size_t i = 0; i < kNumMozcMultipleChoicePrefs; ++i) {
    prefs->RegisterStringPref(
        kMozcMultipleChoicePrefs[i].pref_name,
        kMozcMultipleChoicePrefs[i].default_pref_value);
  }
  for (size_t i = 0; i < kNumMozcIntegerPrefs; ++i) {
    prefs->RegisterIntegerPref(kMozcIntegerPrefs[i].pref_name,
                               kMozcIntegerPrefs[i].default_pref_value);
  }
}
