 void GCInfoTable::EnsureGCInfoIndex(const GCInfo* gc_info,
                                     size_t* gc_info_index_slot) {
   DCHECK(gc_info);
   DCHECK(gc_info_index_slot);
  DEFINE_THREAD_SAFE_STATIC_LOCAL(Mutex, mutex, ());
  MutexLocker locker(mutex);
 
   if (*gc_info_index_slot)
     return;
 
  int index = ++gc_info_index_;
   size_t gc_info_index = static_cast<size_t>(index);
   CHECK(gc_info_index < GCInfoTable::kMaxIndex);
  if (gc_info_index >= gc_info_table_size_)
     Resize();
 
  g_gc_info_table[gc_info_index] = gc_info;
   ReleaseStore(reinterpret_cast<int*>(gc_info_index_slot), index);
 }
