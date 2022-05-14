   void DoTest(ExternalProtocolHandler::BlockState block_state,
               shell_integration::DefaultWebClientState os_state,
               Action expected_action) {
    GURL url("mailto:test@test.com");
     EXPECT_FALSE(delegate_.has_prompted());
     EXPECT_FALSE(delegate_.has_launched());
     EXPECT_FALSE(delegate_.has_blocked());

    delegate_.set_block_state(block_state);
    delegate_.set_os_state(os_state);
    ExternalProtocolHandler::LaunchUrlWithDelegate(
        url, 0, 0, ui::PAGE_TRANSITION_LINK, true, &delegate_);
    content::RunAllTasksUntilIdle();

    EXPECT_EQ(expected_action == Action::PROMPT, delegate_.has_prompted());
    EXPECT_EQ(expected_action == Action::LAUNCH, delegate_.has_launched());
    EXPECT_EQ(expected_action == Action::BLOCK, delegate_.has_blocked());
  }
