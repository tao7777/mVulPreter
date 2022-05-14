  TabManagerTest() : scoped_set_tick_clock_for_testing_(&test_clock_) {
     test_clock_.Advance(kShortDelay);
   }
