  void DetachWebContentsTest(DiscardReason reason) {
    LifecycleUnit* first_lifecycle_unit = nullptr;
    LifecycleUnit* second_lifecycle_unit = nullptr;
    CreateTwoTabs(true /* focus_tab_strip */, &first_lifecycle_unit,
                  &second_lifecycle_unit);

    ExpectCanDiscardTrueAllReasons(first_lifecycle_unit);
    std::unique_ptr<content::WebContents> owned_contents =
        tab_strip_model_->DetachWebContentsAt(0);
    ExpectCanDiscardFalseTrivialAllReasons(first_lifecycle_unit);

    NoUnloadListenerTabStripModelDelegate other_tab_strip_model_delegate;
    TabStripModel other_tab_strip_model(&other_tab_strip_model_delegate,
                                        profile());
     other_tab_strip_model.AddObserver(source_);
 
    EXPECT_CALL(source_observer_, OnLifecycleUnitCreated(::testing::_));
     other_tab_strip_model.AppendWebContents(CreateTestWebContents(),
                                             /*foreground=*/true);
 
    other_tab_strip_model.AppendWebContents(std::move(owned_contents), false);
     ExpectCanDiscardTrueAllReasons(first_lifecycle_unit);
 
     EXPECT_EQ(LifecycleUnitState::ACTIVE, first_lifecycle_unit->GetState());
    EXPECT_CALL(tab_observer_, OnDiscardedStateChange(::testing::_, true));
     first_lifecycle_unit->Discard(reason);
 
    ::testing::Mock::VerifyAndClear(&tab_observer_);
     TransitionFromPendingDiscardToDiscardedIfNeeded(reason,
                                                     first_lifecycle_unit);
 
    CloseTabsAndExpectNotifications(&other_tab_strip_model,
                                    {first_lifecycle_unit});
  }
