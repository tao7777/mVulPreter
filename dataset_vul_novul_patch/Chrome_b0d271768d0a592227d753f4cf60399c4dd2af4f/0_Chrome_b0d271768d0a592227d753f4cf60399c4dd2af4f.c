bool PermissionUtil::GetPermissionType(ContentSettingsType type,
                                       PermissionType* out) {
  if (type == CONTENT_SETTINGS_TYPE_GEOLOCATION) {
    *out = PermissionType::GEOLOCATION;
  } else if (type == CONTENT_SETTINGS_TYPE_NOTIFICATIONS) {
     *out = PermissionType::NOTIFICATIONS;
   } else if (type == CONTENT_SETTINGS_TYPE_PUSH_MESSAGING) {
     *out = PermissionType::PUSH_MESSAGING;
  } else if (type == CONTENT_SETTINGS_TYPE_MIDI) {
    *out = PermissionType::MIDI;
   } else if (type == CONTENT_SETTINGS_TYPE_MIDI_SYSEX) {
     *out = PermissionType::MIDI_SYSEX;
   } else if (type == CONTENT_SETTINGS_TYPE_DURABLE_STORAGE) {
    *out = PermissionType::DURABLE_STORAGE;
  } else if (type == CONTENT_SETTINGS_TYPE_MEDIASTREAM_CAMERA) {
    *out = PermissionType::VIDEO_CAPTURE;
  } else if (type == CONTENT_SETTINGS_TYPE_MEDIASTREAM_MIC) {
    *out = PermissionType::AUDIO_CAPTURE;
  } else if (type == CONTENT_SETTINGS_TYPE_BACKGROUND_SYNC) {
    *out = PermissionType::BACKGROUND_SYNC;
  } else if (type == CONTENT_SETTINGS_TYPE_PLUGINS) {
    *out = PermissionType::FLASH;
#if defined(OS_ANDROID) || defined(OS_CHROMEOS)
  } else if (type == CONTENT_SETTINGS_TYPE_PROTECTED_MEDIA_IDENTIFIER) {
    *out = PermissionType::PROTECTED_MEDIA_IDENTIFIER;
#endif
  } else {
    return false;
  }
  return true;
}
