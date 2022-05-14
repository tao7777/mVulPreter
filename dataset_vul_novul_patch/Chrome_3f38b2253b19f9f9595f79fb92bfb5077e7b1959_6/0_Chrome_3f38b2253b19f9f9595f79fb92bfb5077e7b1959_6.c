  void CreatePersistentHistogramAllocator() {
    allocator_memory_.reset(new char[kAllocatorMemorySize]);
 
     GlobalHistogramAllocator::ReleaseForTesting();
     memset(allocator_memory_.get(), 0, kAllocatorMemorySize);
     GlobalHistogramAllocator::CreateWithPersistentMemory(
         allocator_memory_.get(), kAllocatorMemorySize, 0, 0,
         "PersistentHistogramAllocatorTest");
    allocator_ = GlobalHistogramAllocator::Get()->memory_allocator();
  }
