   void SetPreviewDataForIndex(int index, const base::RefCountedBytes* data) {
    if (index != printing::COMPLETE_PREVIEW_DOCUMENT_INDEX &&
        index < printing::FIRST_PAGE_INDEX) {
       return;
    }
 
     page_data_map_[index] = const_cast<base::RefCountedBytes*>(data);
   }
