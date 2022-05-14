   void CreatePersistentMemoryAllocator() {
     GlobalHistogramAllocator::CreateWithLocalMemory(
         kAllocatorMemorySize, 0, "SparseHistogramAllocatorTest");
     allocator_ = GlobalHistogramAllocator::Get()->memory_allocator();
  }
