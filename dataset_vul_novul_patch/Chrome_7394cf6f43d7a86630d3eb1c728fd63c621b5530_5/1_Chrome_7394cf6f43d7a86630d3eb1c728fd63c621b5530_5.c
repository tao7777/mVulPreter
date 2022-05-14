   void CloseTabsAndExpectNotifications(
       TabStripModel* tab_strip_model,
       std::vector<LifecycleUnit*> lifecycle_units) {
    std::vector<std::unique_ptr<testing::StrictMock<MockLifecycleUnitObserver>>>
         observers;
     for (LifecycleUnit* lifecycle_unit : lifecycle_units) {
       observers.emplace_back(
          std::make_unique<testing::StrictMock<MockLifecycleUnitObserver>>());
       lifecycle_unit->AddObserver(observers.back().get());
       EXPECT_CALL(*observers.back().get(),
                   OnLifecycleUnitDestroyed(lifecycle_unit));
    }
    tab_strip_model->CloseAllTabs();
  }
