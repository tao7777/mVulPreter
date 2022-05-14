   StatisticsRecorderTest() : use_persistent_histogram_allocator_(GetParam()) {
    PersistentHistogramAllocator::GetCreateHistogramResultHistogram();
     InitializeStatisticsRecorder();

    if (use_persistent_histogram_allocator_) {
      GlobalHistogramAllocator::CreateWithLocalMemory(kAllocatorMemorySize, 0,
                                                      "StatisticsRecorderTest");
    }
  }
