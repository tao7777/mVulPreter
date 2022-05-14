bool WebviewInfo::IsResourceWebviewAccessible(
    const Extension* extension,
    const std::string& partition_id,
    const std::string& relative_path) {
  if (!extension)
    return false;

  const WebviewInfo* info = GetResourcesInfo(*extension);
   if (!info)
     return false;
 
  bool partition_is_privileged = false;
  for (size_t i = 0;
       i < info->webview_privileged_partitions_.size();
       ++i) {
    if (MatchPattern(partition_id, info->webview_privileged_partitions_[i])) {
      partition_is_privileged = true;
      break;
     }
   }
 
  return partition_is_privileged && extension->ResourceMatches(
      info->webview_accessible_resources_, relative_path);
 }
