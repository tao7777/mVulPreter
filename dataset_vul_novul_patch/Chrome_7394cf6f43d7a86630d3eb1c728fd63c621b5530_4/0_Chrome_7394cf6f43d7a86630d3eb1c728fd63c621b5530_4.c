  void CanOnlyDiscardOnceTest(DiscardReason reason) {
    LifecycleUnit* background_lifecycle_unit = nullptr;
    LifecycleUnit* foreground_lifecycle_unit = nullptr;
    CreateTwoTabs(true /* focus_tab_strip */, &background_lifecycle_unit,
                  &foreground_lifecycle_unit);
    content::WebContents* initial_web_contents =
        tab_strip_model_->GetWebContentsAt(0);

    ExpectCanDiscardTrueAllReasons(background_lifecycle_unit);

     EXPECT_EQ(LifecycleUnitState::ACTIVE,
               background_lifecycle_unit->GetState());
    EXPECT_CALL(tab_observer_, OnDiscardedStateChange(::testing::_, true));
     background_lifecycle_unit->Discard(reason);
 
    ::testing::Mock::VerifyAndClear(&tab_observer_);
 
     TransitionFromPendingDiscardToDiscardedIfNeeded(reason,
                                                     background_lifecycle_unit);

    EXPECT_NE(initial_web_contents, tab_strip_model_->GetWebContentsAt(0));
    EXPECT_FALSE(tab_strip_model_->GetWebContentsAt(0)
                     ->GetController()
                      .GetPendingEntry());
 
    EXPECT_CALL(tab_observer_, OnDiscardedStateChange(::testing::_, false));
     tab_strip_model_->GetWebContentsAt(0)->GetController().Reload(
         content::ReloadType::NORMAL, false);
    ::testing::Mock::VerifyAndClear(&tab_observer_);
     EXPECT_EQ(LifecycleUnitState::ACTIVE,
               background_lifecycle_unit->GetState());
     EXPECT_TRUE(tab_strip_model_->GetWebContentsAt(0)
                    ->GetController()
                    .GetPendingEntry());

    ExpectCanDiscardFalseTrivial(background_lifecycle_unit,
                                 DiscardReason::kExternal);
    ExpectCanDiscardFalseTrivial(background_lifecycle_unit,
                                 DiscardReason::kProactive);
#if defined(OS_CHROMEOS)
    ExpectCanDiscardTrue(background_lifecycle_unit, DiscardReason::kUrgent);
#else
    ExpectCanDiscardFalseTrivial(background_lifecycle_unit,
                                 DiscardReason::kUrgent);
#endif
   }
