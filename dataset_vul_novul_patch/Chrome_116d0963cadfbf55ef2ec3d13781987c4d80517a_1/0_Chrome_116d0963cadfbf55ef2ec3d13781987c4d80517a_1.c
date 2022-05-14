 void PrintPreviewDataService::GetDataEntry(
    int32 preview_ui_id,
     int index,
     scoped_refptr<base::RefCountedBytes>* data_bytes) {
   *data_bytes = NULL;
  PreviewDataStoreMap::const_iterator it = data_store_map_.find(preview_ui_id);
   if (it != data_store_map_.end())
     it->second->GetPreviewDataForIndex(index, data_bytes);
 }
