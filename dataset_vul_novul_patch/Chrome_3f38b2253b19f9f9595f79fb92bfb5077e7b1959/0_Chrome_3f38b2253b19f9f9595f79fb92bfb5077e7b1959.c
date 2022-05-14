   void CreatePersistentHistogramAllocator() {
     GlobalHistogramAllocator::CreateWithLocalMemory(
         kAllocatorMemorySize, 0, "HistogramAllocatorTest");
     allocator_ = GlobalHistogramAllocator::Get()->memory_allocator();
  }
