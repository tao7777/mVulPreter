void PluginInfoMessageFilter::Context::DecidePluginStatus(
    const GetPluginInfo_Params& params,
    const WebPluginInfo& plugin,
    PluginFinder* plugin_finder,
    ChromeViewHostMsg_GetPluginInfo_Status* status,
    std::string* group_identifier,
    string16* group_name) const {
  PluginInstaller* installer = plugin_finder->GetPluginInstaller(plugin);
  *group_name = installer->name();
  *group_identifier = installer->identifier();

  ContentSetting plugin_setting = CONTENT_SETTING_DEFAULT;
  bool uses_default_content_setting = true;
  GetPluginContentSetting(plugin, params.top_origin_url, params.url,
                          *group_identifier, &plugin_setting,
                          &uses_default_content_setting);
  DCHECK(plugin_setting != CONTENT_SETTING_DEFAULT);

#if defined(ENABLE_PLUGIN_INSTALLATION)
  PluginInstaller::SecurityStatus plugin_status =
      installer->GetSecurityStatus(plugin);
  if (plugin_status == PluginInstaller::SECURITY_STATUS_OUT_OF_DATE &&
      !allow_outdated_plugins_.GetValue()) {
    if (allow_outdated_plugins_.IsManaged()) {
      status->value =
          ChromeViewHostMsg_GetPluginInfo_Status::kOutdatedDisallowed;
    } else {
      status->value = ChromeViewHostMsg_GetPluginInfo_Status::kOutdatedBlocked;
    }
    return;
   }
 
  if (plugin_status ==
          PluginInstaller::SECURITY_STATUS_REQUIRES_AUTHORIZATION &&
       plugin.type != WebPluginInfo::PLUGIN_TYPE_PEPPER_IN_PROCESS &&
       plugin.type != WebPluginInfo::PLUGIN_TYPE_PEPPER_OUT_OF_PROCESS &&
       !always_authorize_plugins_.GetValue() &&
      plugin_setting != CONTENT_SETTING_BLOCK &&
      uses_default_content_setting) {
     status->value = ChromeViewHostMsg_GetPluginInfo_Status::kUnauthorized;
     return;
   }

  // Check if the plug-in is crashing too much.
  if (PluginService::GetInstance()->IsPluginUnstable(plugin.path) &&
      !always_authorize_plugins_.GetValue() &&
      plugin_setting != CONTENT_SETTING_BLOCK &&
      uses_default_content_setting) {
    status->value = ChromeViewHostMsg_GetPluginInfo_Status::kUnauthorized;
    return;
  }
 #endif
 
   if (plugin_setting == CONTENT_SETTING_ASK)
    status->value = ChromeViewHostMsg_GetPluginInfo_Status::kClickToPlay;
  else if (plugin_setting == CONTENT_SETTING_BLOCK)
    status->value = ChromeViewHostMsg_GetPluginInfo_Status::kBlocked;
}
