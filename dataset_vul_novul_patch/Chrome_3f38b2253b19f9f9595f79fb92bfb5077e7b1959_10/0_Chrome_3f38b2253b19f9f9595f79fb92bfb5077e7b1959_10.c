  FileMetricsProviderTest()
      : create_large_files_(GetParam()),
        task_runner_(new base::TestSimpleTaskRunner()),
        thread_task_runner_handle_(task_runner_),
        statistics_recorder_(
            base::StatisticsRecorder::CreateTemporaryForTesting()),
        prefs_(new TestingPrefServiceSimple) {
     EXPECT_TRUE(temp_dir_.CreateUniqueTempDir());
     FileMetricsProvider::RegisterPrefs(prefs_->registry(), kMetricsName);
     FileMetricsProvider::SetTaskRunnerForTesting(task_runner_);
   }
