 void GCInfoTable::EnsureGCInfoIndex(const GCInfo* gc_info,
                                     size_t* gc_info_index_slot) {
   DCHECK(gc_info);
   DCHECK(gc_info_index_slot);

  // Ensuring a new index involves current index adjustment as well
  // as potentially resizing the table, both operations that require
  // a lock.
  MutexLocker locker(table_mutex_);
 
   if (*gc_info_index_slot)
     return;
 
  int index = ++current_index_;
   size_t gc_info_index = static_cast<size_t>(index);
   CHECK(gc_info_index < GCInfoTable::kMaxIndex);
  if (current_index_ >= limit_)
     Resize();
 
  table_[gc_info_index] = gc_info;
   ReleaseStore(reinterpret_cast<int*>(gc_info_index_slot), index);
 }
