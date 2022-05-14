 void GCInfoTable::Resize() {
  static const int kGcInfoZapValue = 0x33;
  const size_t kInitialSize = 512;
  size_t new_size =
      gc_info_table_size_ ? 2 * gc_info_table_size_ : kInitialSize;
  DCHECK(new_size < GCInfoTable::kMaxIndex);
  g_gc_info_table =
      reinterpret_cast<GCInfo const**>(WTF::Partitions::FastRealloc(
          g_gc_info_table, new_size * sizeof(GCInfo), "GCInfo"));
  DCHECK(g_gc_info_table);
  memset(reinterpret_cast<uint8_t*>(g_gc_info_table) +
             gc_info_table_size_ * sizeof(GCInfo),
         kGcInfoZapValue, (new_size - gc_info_table_size_) * sizeof(GCInfo));
  gc_info_table_size_ = new_size;
 }
