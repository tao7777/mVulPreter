   void SetPreviewDataForIndex(int index, const base::RefCountedBytes* data) {
    if (IsInvalidIndex(index))
       return;
 
     page_data_map_[index] = const_cast<base::RefCountedBytes*>(data);
   }
