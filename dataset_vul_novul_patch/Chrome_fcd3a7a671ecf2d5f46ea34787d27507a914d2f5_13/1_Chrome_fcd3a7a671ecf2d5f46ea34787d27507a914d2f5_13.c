  void SetUp() {
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());

    SyncCredentials credentials;
    credentials.email = "foo@bar.com";
    credentials.sync_token = "sometoken";

    sync_notifier_mock_ = new StrictMock<SyncNotifierMock>();
    EXPECT_CALL(*sync_notifier_mock_, AddObserver(_)).
        WillOnce(Invoke(this, &SyncManagerTest::SyncNotifierAddObserver));
    EXPECT_CALL(*sync_notifier_mock_, SetUniqueId(_));
    EXPECT_CALL(*sync_notifier_mock_, SetState(""));
    EXPECT_CALL(*sync_notifier_mock_,
                UpdateCredentials(credentials.email, credentials.sync_token));
    EXPECT_CALL(*sync_notifier_mock_, UpdateEnabledTypes(_)).
        Times(AtLeast(1)).
        WillRepeatedly(
            Invoke(this, &SyncManagerTest::SyncNotifierUpdateEnabledTypes));
    EXPECT_CALL(*sync_notifier_mock_, RemoveObserver(_)).
        WillOnce(Invoke(this, &SyncManagerTest::SyncNotifierRemoveObserver));

    sync_manager_.AddObserver(&observer_);
    EXPECT_CALL(observer_, OnInitializationComplete(_, _)).
        WillOnce(SaveArg<0>(&js_backend_));

    EXPECT_FALSE(sync_notifier_observer_);
    EXPECT_FALSE(js_backend_.IsInitialized());

    sync_manager_.Init(temp_dir_.path(),
                       WeakHandle<JsEventHandler>(),
                       "bogus", 0, false,
                       base::MessageLoopProxy::current(),
                        new TestHttpPostProviderFactory(), this,
                        &extensions_activity_monitor_, this, "bogus",
                        credentials,
                       false /* enable_sync_tabs_for_other_clients */,
                        sync_notifier_mock_, "",
                        sync_api::SyncManager::TEST_IN_MEMORY,
                        &encryptor_,
                       &handler_,
                       NULL);

    EXPECT_TRUE(sync_notifier_observer_);
    EXPECT_TRUE(js_backend_.IsInitialized());

    EXPECT_EQ(1, update_enabled_types_call_count_);

    ModelSafeRoutingInfo routes;
    GetModelSafeRoutingInfo(&routes);
    for (ModelSafeRoutingInfo::iterator i = routes.begin(); i != routes.end();
         ++i) {
      type_roots_[i->first] = MakeServerNodeForType(
          sync_manager_.GetUserShare(), i->first);
    }
    PumpLoop();
  }
