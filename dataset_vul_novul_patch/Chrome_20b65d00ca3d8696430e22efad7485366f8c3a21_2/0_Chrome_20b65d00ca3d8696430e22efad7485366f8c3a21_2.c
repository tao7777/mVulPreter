void ThreadHeap::TakeSnapshot(SnapshotType type) {
  DCHECK(thread_state_->InAtomicMarkingPause());
 
  ThreadState::GCSnapshotInfo info(GCInfoTable::Get().GcInfoIndex() + 1);
   String thread_dump_name =
       String::Format("blink_gc/thread_%lu",
                      static_cast<unsigned long>(thread_state_->ThreadId()));
  const String heaps_dump_name = thread_dump_name + "/heaps";
  const String classes_dump_name = thread_dump_name + "/classes";

  int number_of_heaps_reported = 0;
#define SNAPSHOT_HEAP(ArenaType)                                          \
  {                                                                       \
    number_of_heaps_reported++;                                           \
    switch (type) {                                                       \
      case SnapshotType::kHeapSnapshot:                                   \
        arenas_[BlinkGC::k##ArenaType##ArenaIndex]->TakeSnapshot(         \
            heaps_dump_name + "/" #ArenaType, info);                      \
        break;                                                            \
      case SnapshotType::kFreelistSnapshot:                               \
        arenas_[BlinkGC::k##ArenaType##ArenaIndex]->TakeFreelistSnapshot( \
            heaps_dump_name + "/" #ArenaType);                            \
        break;                                                            \
      default:                                                            \
        NOTREACHED();                                                     \
    }                                                                     \
  }

  SNAPSHOT_HEAP(NormalPage1);
  SNAPSHOT_HEAP(NormalPage2);
  SNAPSHOT_HEAP(NormalPage3);
  SNAPSHOT_HEAP(NormalPage4);
  SNAPSHOT_HEAP(EagerSweep);
  SNAPSHOT_HEAP(Vector1);
  SNAPSHOT_HEAP(Vector2);
  SNAPSHOT_HEAP(Vector3);
  SNAPSHOT_HEAP(Vector4);
  SNAPSHOT_HEAP(InlineVector);
  SNAPSHOT_HEAP(HashTable);
  SNAPSHOT_HEAP(LargeObject);
  FOR_EACH_TYPED_ARENA(SNAPSHOT_HEAP);

  DCHECK_EQ(number_of_heaps_reported, BlinkGC::kNumberOfArenas);

#undef SNAPSHOT_HEAP

  if (type == SnapshotType::kFreelistSnapshot)
    return;

  size_t total_live_count = 0;
   size_t total_dead_count = 0;
   size_t total_live_size = 0;
   size_t total_dead_size = 0;
  for (size_t gc_info_index = 1;
       gc_info_index <= GCInfoTable::Get().GcInfoIndex(); ++gc_info_index) {
     total_live_count += info.live_count[gc_info_index];
     total_dead_count += info.dead_count[gc_info_index];
     total_live_size += info.live_size[gc_info_index];
    total_dead_size += info.dead_size[gc_info_index];
  }

  base::trace_event::MemoryAllocatorDump* thread_dump =
      BlinkGCMemoryDumpProvider::Instance()
          ->CreateMemoryAllocatorDumpForCurrentGC(thread_dump_name);
  thread_dump->AddScalar("live_count", "objects", total_live_count);
  thread_dump->AddScalar("dead_count", "objects", total_dead_count);
  thread_dump->AddScalar("live_size", "bytes", total_live_size);
  thread_dump->AddScalar("dead_size", "bytes", total_dead_size);

  base::trace_event::MemoryAllocatorDump* heaps_dump =
      BlinkGCMemoryDumpProvider::Instance()
          ->CreateMemoryAllocatorDumpForCurrentGC(heaps_dump_name);
  base::trace_event::MemoryAllocatorDump* classes_dump =
      BlinkGCMemoryDumpProvider::Instance()
          ->CreateMemoryAllocatorDumpForCurrentGC(classes_dump_name);
  BlinkGCMemoryDumpProvider::Instance()
      ->CurrentProcessMemoryDump()
      ->AddOwnershipEdge(classes_dump->guid(), heaps_dump->guid());
}
