bool ScreenLayoutObserver::GetDisplayMessageForNotification(
    const ScreenLayoutObserver::DisplayInfoMap& old_info,
    base::string16* out_message,
    base::string16* out_additional_message) {
  if (old_display_mode_ != current_display_mode_) {
    if (current_display_mode_ == DisplayMode::MIRRORING) {
      *out_message = GetEnterMirrorModeMessage();
      return true;
    }
    if (old_display_mode_ == DisplayMode::MIRRORING &&
        GetExitMirrorModeMessage(out_message, out_additional_message)) {
      return true;
    }

    if (current_display_mode_ == DisplayMode::UNIFIED) {
      *out_message = GetEnterUnifiedModeMessage();
      return true;
    }
    if (old_display_mode_ == DisplayMode::UNIFIED) {
      *out_message = GetExitUnifiedModeMessage();
      return true;
    }

    if (current_display_mode_ == DisplayMode::DOCKED ||
        old_display_mode_ == DisplayMode::DOCKED) {
      return false;
    }
  }

  if (display_info_.size() < old_info.size()) {
    for (const auto& iter : old_info) {
      if (display_info_.count(iter.first))
        continue;

      *out_message =
          GetDisplayRemovedMessage(iter.second, out_additional_message);
      return true;
    }
  } else if (display_info_.size() > old_info.size()) {
    for (const auto& iter : display_info_) {
      if (old_info.count(iter.first))
        continue;

      *out_message = GetDisplayAddedMessage(iter.first, out_additional_message);
      return true;
    }
  }

  for (const auto& iter : display_info_) {
    DisplayInfoMap::const_iterator old_iter = old_info.find(iter.first);
    if (old_iter == old_info.end()) {
      NOTREACHED() << "A display mode transition that should have been handled"
                      "earlier.";
      return false;
    }

    if (iter.second.configured_ui_scale() !=
        old_iter->second.configured_ui_scale()) {
      *out_additional_message = l10n_util::GetStringFUTF16(
          IDS_ASH_STATUS_TRAY_DISPLAY_RESOLUTION_CHANGED,
           GetDisplayName(iter.first), GetDisplaySize(iter.first));
       return true;
     }
    // We don't show rotation change notification when the rotation source is
    // the accelerometer.
    if (iter.second.active_rotation_source() !=
            display::Display::ROTATION_SOURCE_ACCELEROMETER &&
        iter.second.GetActiveRotation() !=
            old_iter->second.GetActiveRotation()) {
       int rotation_text_id = 0;
       switch (iter.second.GetActiveRotation()) {
         case display::Display::ROTATE_0:
          rotation_text_id = IDS_ASH_STATUS_TRAY_DISPLAY_STANDARD_ORIENTATION;
          break;
        case display::Display::ROTATE_90:
          rotation_text_id = IDS_ASH_STATUS_TRAY_DISPLAY_ORIENTATION_90;
          break;
        case display::Display::ROTATE_180:
          rotation_text_id = IDS_ASH_STATUS_TRAY_DISPLAY_ORIENTATION_180;
          break;
        case display::Display::ROTATE_270:
          rotation_text_id = IDS_ASH_STATUS_TRAY_DISPLAY_ORIENTATION_270;
          break;
      }
      *out_additional_message = l10n_util::GetStringFUTF16(
          IDS_ASH_STATUS_TRAY_DISPLAY_ROTATED, GetDisplayName(iter.first),
          l10n_util::GetStringUTF16(rotation_text_id));
      return true;
    }
  }

  return false;
}
