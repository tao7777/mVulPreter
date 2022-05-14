void CreatePrintSettingsDictionary(DictionaryValue* dict) {
  dict->SetBoolean(printing::kSettingLandscape, false);
  dict->SetBoolean(printing::kSettingCollate, false);
  dict->SetInteger(printing::kSettingColor, printing::GRAY);
  dict->SetBoolean(printing::kSettingPrintToPDF, true);
   dict->SetInteger(printing::kSettingDuplexMode, printing::SIMPLEX);
   dict->SetInteger(printing::kSettingCopies, 1);
   dict->SetString(printing::kSettingDeviceName, "dummy");
  dict->SetString(printing::kPreviewUIAddr, "0xb33fbeef");
   dict->SetInteger(printing::kPreviewRequestID, 12345);
   dict->SetBoolean(printing::kIsFirstRequest, true);
   dict->SetInteger(printing::kSettingMarginsType, printing::DEFAULT_MARGINS);
  dict->SetBoolean(printing::kSettingPreviewModifiable, false);
  dict->SetBoolean(printing::kSettingHeaderFooterEnabled, false);
  dict->SetBoolean(printing::kSettingGenerateDraftData, true);
}
