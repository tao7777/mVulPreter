bool WebviewInfo::IsResourceWebviewAccessible(
    const Extension* extension,
    const std::string& partition_id,
    const std::string& relative_path) {
  if (!extension)
    return false;

  const WebviewInfo* info = GetResourcesInfo(*extension);
   if (!info)
     return false;
 
  for (size_t i = 0; i < info->partition_items_.size(); ++i) {
    const PartitionItem* const item = info->partition_items_[i];
    if (item->Matches(partition_id) &&
        extension->ResourceMatches(item->accessible_resources(),
                                   relative_path)) {
      return true;
     }
   }
 
  return false;
}

void WebviewInfo::AddPartitionItem(scoped_ptr<PartitionItem> item) {
  partition_items_.push_back(item.release());
 }
