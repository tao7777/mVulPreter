std::string ChromeOSGetKeyboardOverlayId(const std::string& input_method_id) {
  for (size_t i = 0; i < arraysize(kKeyboardOverlayMap); ++i) {
    if (kKeyboardOverlayMap[i].input_method_id == input_method_id) {
      return kKeyboardOverlayMap[i].keyboard_overlay_id;
    }
   }
  return "";
}
