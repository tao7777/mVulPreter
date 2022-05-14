 void GCInfoTable::Resize() {
  const size_t new_limit = (limit_) ? 2 * limit_ : ComputeInitialTableLimit();
  const size_t old_committed_size = limit_ * kEntrySize;
  const size_t new_committed_size = new_limit * kEntrySize;
  CHECK(table_);
  CHECK_EQ(0u, new_committed_size % base::kPageAllocationGranularity);
  CHECK_GE(MaxTableSize(), limit_ * kEntrySize);

  // Recommitting and zapping assumes byte-addressable storage.
  uint8_t* const current_table_end =
      reinterpret_cast<uint8_t*>(table_) + old_committed_size;
  const size_t table_size_delta = new_committed_size - old_committed_size;

  // Commit the new size and allow read/write.
  // TODO(ajwong): SetSystemPagesAccess should be part of RecommitSystemPages to
  // avoid having two calls here.
  bool ok = base::SetSystemPagesAccess(current_table_end, table_size_delta,
                                       base::PageReadWrite);
  CHECK(ok);
  ok = base::RecommitSystemPages(current_table_end, table_size_delta,
                                 base::PageReadWrite);
  CHECK(ok);

  // Zap unused values.,
  memset(current_table_end, kGcInfoZapValue, table_size_delta);

  limit_ = new_limit;
 }
