int PrintPreviewDataService::GetAvailableDraftPageCount(
int PrintPreviewDataService::GetAvailableDraftPageCount(int32 preview_ui_id) {
  PreviewDataStoreMap::const_iterator it = data_store_map_.find(preview_ui_id);
  return (it == data_store_map_.end()) ?
      0 : it->second->GetAvailableDraftPageCount();
 }
