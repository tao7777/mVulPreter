 void PrintPreviewDataService::SetDataEntry(
    int32 preview_ui_id,
     int index,
     const base::RefCountedBytes* data_bytes) {
  if (!ContainsKey(data_store_map_, preview_ui_id))
    data_store_map_[preview_ui_id] = new PrintPreviewDataStore();
 
  data_store_map_[preview_ui_id]->SetPreviewDataForIndex(index, data_bytes);
 }
