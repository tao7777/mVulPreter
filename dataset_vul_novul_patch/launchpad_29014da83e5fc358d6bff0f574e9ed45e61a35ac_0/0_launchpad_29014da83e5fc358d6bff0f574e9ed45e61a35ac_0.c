oxide::BrowserContext* WebContext::GetContext() {
BrowserContext* WebContext::GetContext() {
   if (context_.get()) {
     return context_.get();
   }
 
   DCHECK(construct_props_);
 
  BrowserContext::Params params(
       construct_props_->data_path,
       construct_props_->cache_path,
       construct_props_->max_cache_size_hint,
       construct_props_->session_cookie_mode);
   params.host_mapping_rules = construct_props_->host_mapping_rules;
 
  context_ = BrowserContext::Create(params);
 
   UserAgentSettings* ua_settings = UserAgentSettings::Get(context_.get());
  if (!construct_props_->product.empty()) {
    ua_settings->SetProduct(construct_props_->product);
  }
  if (!construct_props_->user_agent.empty()) {
    ua_settings->SetUserAgent(construct_props_->user_agent);
  }
  if (!construct_props_->accept_langs.empty()) {
    ua_settings->SetAcceptLangs(construct_props_->accept_langs);
  }
  ua_settings->SetUserAgentOverrides(construct_props_->user_agent_overrides);
  ua_settings->SetLegacyUserAgentOverrideEnabled(
      construct_props_->legacy_user_agent_override_enabled);

  context_->SetCookiePolicy(construct_props_->cookie_policy);
  context_->SetIsPopupBlockerEnabled(construct_props_->popup_blocker_enabled);
  context_->SetDoNotTrack(construct_props_->do_not_track);

  MediaCaptureDevicesContext* dc =
      MediaCaptureDevicesContext::Get(context_.get());

  if (!construct_props_->default_audio_capture_device_id.empty()) {
    if (!dc->SetDefaultAudioDeviceId(
        construct_props_->default_audio_capture_device_id)) {
      client_->DefaultAudioCaptureDeviceChanged();
    }
  }
  if (!construct_props_->default_video_capture_device_id.empty()) {
    if (!dc->SetDefaultVideoDeviceId(
        construct_props_->default_video_capture_device_id)) {
      client_->DefaultVideoCaptureDeviceChanged();
    }
  }

  dc->set_client(this);

  DevToolsManager* devtools = DevToolsManager::Get(context_.get());
  if (!construct_props_->devtools_ip.empty()) {
    devtools->SetAddress(construct_props_->devtools_ip);
  }
  if (construct_props_->devtools_port != -1) {
    devtools->SetPort(construct_props_->devtools_port);
  }
  devtools->SetEnabled(construct_props_->devtools_enabled);

  context_->SetDelegate(delegate_.get());

  construct_props_.reset();

  UpdateUserScripts();

  return context_.get();
}
