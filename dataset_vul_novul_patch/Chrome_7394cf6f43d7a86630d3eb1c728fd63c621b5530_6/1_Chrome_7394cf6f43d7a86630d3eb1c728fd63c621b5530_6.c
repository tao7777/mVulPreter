  void CreateTwoTabs(bool focus_tab_strip,
                     LifecycleUnit** first_lifecycle_unit,
                     LifecycleUnit** second_lifecycle_unit) {
    if (focus_tab_strip)
      source_->SetFocusedTabStripModelForTesting(tab_strip_model_.get());

     task_runner_->FastForwardBy(kShortDelay);
     auto time_before_first_tab = NowTicks();
    EXPECT_CALL(source_observer_, OnLifecycleUnitCreated(testing::_))
        .WillOnce(testing::Invoke([&](LifecycleUnit* lifecycle_unit) {
           *first_lifecycle_unit = lifecycle_unit;
 
           if (focus_tab_strip) {
            EXPECT_TRUE(IsFocused(*first_lifecycle_unit));
          } else {
            EXPECT_EQ(time_before_first_tab,
                      (*first_lifecycle_unit)->GetLastFocusedTime());
          }
        }));
    std::unique_ptr<content::WebContents> first_web_contents =
         CreateAndNavigateWebContents();
     content::WebContents* raw_first_web_contents = first_web_contents.get();
     tab_strip_model_->AppendWebContents(std::move(first_web_contents), true);
    testing::Mock::VerifyAndClear(&source_observer_);
     EXPECT_TRUE(source_->GetTabLifecycleUnitExternal(raw_first_web_contents));
 
     task_runner_->FastForwardBy(kShortDelay);
     auto time_before_second_tab = NowTicks();
    EXPECT_CALL(source_observer_, OnLifecycleUnitCreated(testing::_))
        .WillOnce(testing::Invoke([&](LifecycleUnit* lifecycle_unit) {
           *second_lifecycle_unit = lifecycle_unit;
 
           if (focus_tab_strip) {
            EXPECT_EQ(time_before_second_tab,
                      (*first_lifecycle_unit)->GetLastFocusedTime());
            EXPECT_TRUE(IsFocused(*second_lifecycle_unit));
          } else {
            EXPECT_EQ(time_before_first_tab,
                      (*first_lifecycle_unit)->GetLastFocusedTime());
            EXPECT_EQ(time_before_second_tab,
                      (*second_lifecycle_unit)->GetLastFocusedTime());
          }
        }));
    std::unique_ptr<content::WebContents> second_web_contents =
         CreateAndNavigateWebContents();
     content::WebContents* raw_second_web_contents = second_web_contents.get();
     tab_strip_model_->AppendWebContents(std::move(second_web_contents), true);
    testing::Mock::VerifyAndClear(&source_observer_);
     EXPECT_TRUE(source_->GetTabLifecycleUnitExternal(raw_second_web_contents));
 
     raw_first_web_contents->WasHidden();
  }
