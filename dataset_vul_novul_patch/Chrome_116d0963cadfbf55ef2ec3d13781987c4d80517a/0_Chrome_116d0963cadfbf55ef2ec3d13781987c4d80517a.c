   int GetAvailableDraftPageCount() {
     int page_data_map_size = page_data_map_.size();
    if (ContainsKey(page_data_map_, printing::COMPLETE_PREVIEW_DOCUMENT_INDEX))
       page_data_map_size--;
     return page_data_map_size;
   }
