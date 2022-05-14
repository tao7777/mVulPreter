   void GetPreviewDataForIndex(int index,
                               scoped_refptr<base::RefCountedBytes>* data) {
    if (IsInvalidIndex(index))
       return;
 
     PreviewPageDataMap::iterator it = page_data_map_.find(index);
     if (it != page_data_map_.end())
      *data = it->second.get();
  }
