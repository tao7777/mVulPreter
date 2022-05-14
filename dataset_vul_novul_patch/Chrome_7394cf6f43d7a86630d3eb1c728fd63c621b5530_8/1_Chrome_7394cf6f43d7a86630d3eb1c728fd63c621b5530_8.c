  void DiscardAndActivateTest(DiscardReason reason) {
    LifecycleUnit* background_lifecycle_unit = nullptr;
    LifecycleUnit* foreground_lifecycle_unit = nullptr;
    CreateTwoTabs(true /* focus_tab_strip */, &background_lifecycle_unit,
                  &foreground_lifecycle_unit);
    content::WebContents* initial_web_contents =
        tab_strip_model_->GetWebContentsAt(0);

     EXPECT_EQ(LifecycleUnitState::ACTIVE,
               background_lifecycle_unit->GetState());
    EXPECT_CALL(tab_observer_, OnDiscardedStateChange(testing::_, true));
     background_lifecycle_unit->Discard(reason);
    testing::Mock::VerifyAndClear(&tab_observer_);
 
     TransitionFromPendingDiscardToDiscardedIfNeeded(reason,
                                                     background_lifecycle_unit);

    EXPECT_NE(initial_web_contents, tab_strip_model_->GetWebContentsAt(0));
    EXPECT_FALSE(tab_strip_model_->GetWebContentsAt(0)
                     ->GetController()
                      .GetPendingEntry());
 
    EXPECT_CALL(tab_observer_, OnDiscardedStateChange(testing::_, false));
     tab_strip_model_->ActivateTabAt(0, true);
    testing::Mock::VerifyAndClear(&tab_observer_);
     EXPECT_EQ(LifecycleUnitState::ACTIVE,
               background_lifecycle_unit->GetState());
     EXPECT_TRUE(tab_strip_model_->GetWebContentsAt(0)
                    ->GetController()
                    .GetPendingEntry());
  }
