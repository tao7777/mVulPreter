   void GetPreviewDataForIndex(int index,
                               scoped_refptr<base::RefCountedBytes>* data) {
    if (index != printing::COMPLETE_PREVIEW_DOCUMENT_INDEX &&
        index < printing::FIRST_PAGE_INDEX) {
       return;
    }
 
     PreviewPageDataMap::iterator it = page_data_map_.find(index);
     if (it != page_data_map_.end())
      *data = it->second.get();
  }
