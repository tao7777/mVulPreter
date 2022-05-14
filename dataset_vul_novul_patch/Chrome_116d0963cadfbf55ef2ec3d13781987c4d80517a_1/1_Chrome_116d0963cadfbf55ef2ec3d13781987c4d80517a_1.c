 void PrintPreviewDataService::GetDataEntry(
    const std::string& preview_ui_addr_str,
     int index,
     scoped_refptr<base::RefCountedBytes>* data_bytes) {
   *data_bytes = NULL;
  PreviewDataStoreMap::iterator it = data_store_map_.find(preview_ui_addr_str);
   if (it != data_store_map_.end())
     it->second->GetPreviewDataForIndex(index, data_bytes);
 }
