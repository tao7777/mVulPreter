 bool MockContentSettingsClient::allowAutoplay(bool default_value) {
  return default_value || flags_->autoplay_allowed();
 }
