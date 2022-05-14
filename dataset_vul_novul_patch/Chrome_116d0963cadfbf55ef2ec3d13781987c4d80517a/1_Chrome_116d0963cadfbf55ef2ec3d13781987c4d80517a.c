   int GetAvailableDraftPageCount() {
     int page_data_map_size = page_data_map_.size();
    if (page_data_map_.find(printing::COMPLETE_PREVIEW_DOCUMENT_INDEX) !=
        page_data_map_.end()) {
       page_data_map_size--;
    }
     return page_data_map_size;
   }
