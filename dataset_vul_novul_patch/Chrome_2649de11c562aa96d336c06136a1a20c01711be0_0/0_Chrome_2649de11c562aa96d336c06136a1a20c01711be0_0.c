void ExtensionApiTest::SetUpOnMainThread() {
  ExtensionBrowserTest::SetUpOnMainThread();
  DCHECK(!test_config_.get()) << "Previous test did not clear config state.";
  test_config_.reset(new base::DictionaryValue());
  test_config_->SetString(kTestDataDirectory,
                           net::FilePathToFileURL(test_data_dir_).spec());
   test_config_->SetBoolean(kBrowserSideNavigationEnabled,
                            content::IsBrowserSideNavigationEnabled());
  if (embedded_test_server()->Started()) {
    // InitializeEmbeddedTestServer was called before |test_config_| was set.
    // Set the missing port key.
    test_config_->SetInteger(kEmbeddedTestServerPort,
                             embedded_test_server()->port());
  }
   extensions::TestGetConfigFunction::set_test_config_state(
       test_config_.get());
 }
