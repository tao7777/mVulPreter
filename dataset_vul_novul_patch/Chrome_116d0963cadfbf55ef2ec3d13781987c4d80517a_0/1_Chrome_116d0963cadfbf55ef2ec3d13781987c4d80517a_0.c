int PrintPreviewDataService::GetAvailableDraftPageCount(
    const std::string& preview_ui_addr_str) {
  if (data_store_map_.find(preview_ui_addr_str) != data_store_map_.end())
    return data_store_map_[preview_ui_addr_str]->GetAvailableDraftPageCount();
  return 0;
 }
