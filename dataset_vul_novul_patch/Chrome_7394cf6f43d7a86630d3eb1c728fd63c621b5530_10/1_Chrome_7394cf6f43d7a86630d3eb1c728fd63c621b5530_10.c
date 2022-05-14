  void DiscardTest(DiscardReason reason) {
    const base::TimeTicks kDummyLastActiveTime =
        base::TimeTicks() + kShortDelay;

    LifecycleUnit* background_lifecycle_unit = nullptr;
    LifecycleUnit* foreground_lifecycle_unit = nullptr;
    CreateTwoTabs(true /* focus_tab_strip */, &background_lifecycle_unit,
                  &foreground_lifecycle_unit);
    content::WebContents* initial_web_contents =
        tab_strip_model_->GetWebContentsAt(0);
    content::WebContentsTester::For(initial_web_contents)
        ->SetLastActiveTime(kDummyLastActiveTime);

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
    EXPECT_EQ(kDummyLastActiveTime,
              tab_strip_model_->GetWebContentsAt(0)->GetLastActiveTime());

    source_->SetFocusedTabStripModelForTesting(nullptr);
  }
