  void TestAppendTabsToTabStrip(bool focus_tab_strip) {
    LifecycleUnit* first_lifecycle_unit = nullptr;
    LifecycleUnit* second_lifecycle_unit = nullptr;
    CreateTwoTabs(focus_tab_strip, &first_lifecycle_unit,
                  &second_lifecycle_unit);

    const base::TimeTicks first_tab_last_focused_time =
        first_lifecycle_unit->GetLastFocusedTime();
    const base::TimeTicks second_tab_last_focused_time =
        second_lifecycle_unit->GetLastFocusedTime();

     task_runner_->FastForwardBy(kShortDelay);
     LifecycleUnit* third_lifecycle_unit = nullptr;
    EXPECT_CALL(source_observer_, OnLifecycleUnitCreated(testing::_))
        .WillOnce(testing::Invoke([&](LifecycleUnit* lifecycle_unit) {
           third_lifecycle_unit = lifecycle_unit;
 
           if (focus_tab_strip) {
            EXPECT_EQ(first_tab_last_focused_time,
                      first_lifecycle_unit->GetLastFocusedTime());
            EXPECT_TRUE(IsFocused(second_lifecycle_unit));
          } else {
            EXPECT_EQ(first_tab_last_focused_time,
                      first_lifecycle_unit->GetLastFocusedTime());
            EXPECT_EQ(second_tab_last_focused_time,
                      second_lifecycle_unit->GetLastFocusedTime());
          }
          EXPECT_EQ(NowTicks(), third_lifecycle_unit->GetLastFocusedTime());
        }));
    std::unique_ptr<content::WebContents> third_web_contents =
         CreateAndNavigateWebContents();
     content::WebContents* raw_third_web_contents = third_web_contents.get();
     tab_strip_model_->AppendWebContents(std::move(third_web_contents), false);
    testing::Mock::VerifyAndClear(&source_observer_);
     EXPECT_TRUE(source_->GetTabLifecycleUnitExternal(raw_third_web_contents));
 
    CloseTabsAndExpectNotifications(
        tab_strip_model_.get(),
        {first_lifecycle_unit, second_lifecycle_unit, third_lifecycle_unit});
  }
