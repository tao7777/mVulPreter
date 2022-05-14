   ChromeInvalidationClientTest()
      : fake_push_client_(new notifier::FakePushClient()),
        client_(scoped_ptr<notifier::PushClient>(fake_push_client_)),
        kBookmarksId_(kChromeSyncSourceId, "BOOKMARK"),
         kPreferencesId_(kChromeSyncSourceId, "PREFERENCE"),
         kExtensionsId_(kChromeSyncSourceId, "EXTENSION"),
        kAppsId_(kChromeSyncSourceId, "APP") {}
