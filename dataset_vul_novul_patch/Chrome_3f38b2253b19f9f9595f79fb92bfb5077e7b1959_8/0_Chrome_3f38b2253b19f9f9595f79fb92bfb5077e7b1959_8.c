   StatisticsRecorderTest() : use_persistent_histogram_allocator_(GetParam()) {
     InitializeStatisticsRecorder();

    if (use_persistent_histogram_allocator_) {
      GlobalHistogramAllocator::CreateWithLocalMemory(kAllocatorMemorySize, 0,
                                                      "StatisticsRecorderTest");
    }
  }
