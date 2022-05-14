   std::unique_ptr<FakeMediaStreamUIProxy> CreateMockUI(bool expect_started) {
     std::unique_ptr<MockMediaStreamUIProxy> fake_ui =
         std::make_unique<MockMediaStreamUIProxy>();
     if (expect_started)
       EXPECT_CALL(*fake_ui, MockOnStarted(_));
     return fake_ui;
  }
