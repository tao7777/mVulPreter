   SubprocessMetricsProviderTest()
       : thread_bundle_(content::TestBrowserThreadBundle::DEFAULT) {
    base::PersistentHistogramAllocator::GetCreateHistogramResultHistogram();
    provider_.MergeHistogramDeltas();

    test_recorder_ = base::StatisticsRecorder::CreateTemporaryForTesting();

    base::GlobalHistogramAllocator::CreateWithLocalMemory(TEST_MEMORY_SIZE,
                                                          0, "");
  }
