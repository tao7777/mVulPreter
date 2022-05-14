void PluginInfoMessageFilter::Context::GrantAccess(
     const ChromeViewHostMsg_GetPluginInfo_Status& status,
     const FilePath& path) const {
   if (status.value == ChromeViewHostMsg_GetPluginInfo_Status::kAllowed ||
      status.value == ChromeViewHostMsg_GetPluginInfo_Status::kClickToPlay) {
    ChromePluginServiceFilter::GetInstance()->AuthorizePlugin(
        render_process_id_, path);
  }
}
