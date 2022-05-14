GlobalHistogramAllocator::ReleaseForTesting() {
  GlobalHistogramAllocator* histogram_allocator = Get();
  if (!histogram_allocator)
    return nullptr;
  PersistentMemoryAllocator* memory_allocator =
      histogram_allocator->memory_allocator();

  PersistentMemoryAllocator::Iterator iter(memory_allocator);
   const PersistentHistogramData* data;
   while ((data = iter.GetNextOfObject<PersistentHistogramData>()) != nullptr) {
     StatisticsRecorder::ForgetHistogramForTesting(data->name);
    DCHECK_NE(kResultHistogram, data->name);
   }
 
   subtle::Release_Store(&g_histogram_allocator, 0);
  return WrapUnique(histogram_allocator);
};
