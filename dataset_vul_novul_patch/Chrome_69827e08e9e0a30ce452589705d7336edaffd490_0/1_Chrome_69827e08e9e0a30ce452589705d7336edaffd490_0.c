bool MediaStreamDevicesController::IsRequestAllowedByDefault() const {
  if (ShouldAlwaysAllowOrigin())
    return true;

  struct {
    bool has_capability;
    const char* policy_name;
    const char* list_policy_name;
    ContentSettingsType settings_type;
  } device_checks[] = {
    { microphone_requested_, prefs::kAudioCaptureAllowed,
      prefs::kAudioCaptureAllowedUrls, CONTENT_SETTINGS_TYPE_MEDIASTREAM_MIC },
    { webcam_requested_, prefs::kVideoCaptureAllowed,
      prefs::kVideoCaptureAllowedUrls,
      CONTENT_SETTINGS_TYPE_MEDIASTREAM_CAMERA },
  };

  for (size_t i = 0; i < ARRAYSIZE_UNSAFE(device_checks); ++i) {
    if (!device_checks[i].has_capability)
      continue;
 
     DevicePolicy policy = GetDevicePolicy(device_checks[i].policy_name,
                                           device_checks[i].list_policy_name);
    if (policy == ALWAYS_DENY ||
        (policy == POLICY_NOT_SET &&
         profile_->GetHostContentSettingsMap()->GetContentSetting(
            request_.security_origin, request_.security_origin,
            device_checks[i].settings_type, NO_RESOURCE_IDENTIFIER) !=
         CONTENT_SETTING_ALLOW)) {
       return false;
     }
  }

  return true;
}
