void GCInfoTable::Init() {
GCInfoTable::GCInfoTable() {
  CHECK(!table_);
  table_ = reinterpret_cast<GCInfo const**>(base::AllocPages(
      nullptr, MaxTableSize(), base::kPageAllocationGranularity,
      base::PageInaccessible));
  CHECK(table_);
   Resize();
 }
