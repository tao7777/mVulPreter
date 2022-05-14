   std::unique_ptr<FakeMediaStreamUIProxy> CreateMockUI(bool expect_started) {
     std::unique_ptr<MockMediaStreamUIProxy> fake_ui =
         std::make_unique<MockMediaStreamUIProxy>();
    testing::Mock::AllowLeak(fake_ui.get());
     if (expect_started)
       EXPECT_CALL(*fake_ui, MockOnStarted(_));
     return fake_ui;
  }
