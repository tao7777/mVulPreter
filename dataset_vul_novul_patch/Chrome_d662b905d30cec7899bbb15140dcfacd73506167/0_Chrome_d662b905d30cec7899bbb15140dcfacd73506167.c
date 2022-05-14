BlockedPluginInfoBarDelegate::BlockedPluginInfoBarDelegate(
    TabContents* tab_contents,
    const string16& utf16_name)
    : PluginInfoBarDelegate(tab_contents, utf16_name) {
  UserMetrics::RecordAction(UserMetricsAction("BlockedPluginInfobar.Shown"));
  std::string name = UTF16ToUTF8(utf16_name);
  if (name == webkit::npapi::PluginGroup::kJavaGroupName)
    UserMetrics::RecordAction(
        UserMetricsAction("BlockedPluginInfobar.Shown.Java"));
  else if (name == webkit::npapi::PluginGroup::kQuickTimeGroupName)
    UserMetrics::RecordAction(
        UserMetricsAction("BlockedPluginInfobar.Shown.QuickTime"));
  else if (name == webkit::npapi::PluginGroup::kShockwaveGroupName)
    UserMetrics::RecordAction(
        UserMetricsAction("BlockedPluginInfobar.Shown.Shockwave"));
   else if (name == webkit::npapi::PluginGroup::kRealPlayerGroupName)
     UserMetrics::RecordAction(
         UserMetricsAction("BlockedPluginInfobar.Shown.RealPlayer"));
  else if (name == webkit::npapi::PluginGroup::kWindowsMediaPlayerGroupName)
    UserMetrics::RecordAction(
        UserMetricsAction("BlockedPluginInfobar.Shown.WindowsMediaPlayer"));
 }
