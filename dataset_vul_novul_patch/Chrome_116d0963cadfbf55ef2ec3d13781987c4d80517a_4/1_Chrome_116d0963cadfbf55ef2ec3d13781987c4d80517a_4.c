 void PrintPreviewDataService::SetDataEntry(
    const std::string& preview_ui_addr_str,
     int index,
     const base::RefCountedBytes* data_bytes) {
  PreviewDataStoreMap::iterator it = data_store_map_.find(preview_ui_addr_str);
  if (it == data_store_map_.end())
    data_store_map_[preview_ui_addr_str] = new PrintPreviewDataStore();
 
  data_store_map_[preview_ui_addr_str]->SetPreviewDataForIndex(index,
                                                               data_bytes);
 }
