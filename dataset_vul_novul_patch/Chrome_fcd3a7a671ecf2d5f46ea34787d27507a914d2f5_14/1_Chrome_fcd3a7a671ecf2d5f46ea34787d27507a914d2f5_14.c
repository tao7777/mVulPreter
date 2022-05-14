DictionaryValue* NigoriSpecificsToValue(
    const sync_pb::NigoriSpecifics& proto) {
  DictionaryValue* value = new DictionaryValue();
  SET(encrypted, EncryptedDataToValue);
  SET_BOOL(using_explicit_passphrase);
  SET_BOOL(encrypt_bookmarks);
  SET_BOOL(encrypt_preferences);
  SET_BOOL(encrypt_autofill_profile);
  SET_BOOL(encrypt_autofill);
  SET_BOOL(encrypt_themes);
  SET_BOOL(encrypt_typed_urls);
  SET_BOOL(encrypt_extension_settings);
  SET_BOOL(encrypt_extensions);
  SET_BOOL(encrypt_sessions);
   SET_BOOL(encrypt_app_settings);
   SET_BOOL(encrypt_apps);
   SET_BOOL(encrypt_search_engines);
  SET_BOOL(sync_tabs);
   SET_BOOL(encrypt_everything);
   SET_REP(device_information, DeviceInformationToValue);
   SET_BOOL(sync_tab_favicons);
  return value;
}
