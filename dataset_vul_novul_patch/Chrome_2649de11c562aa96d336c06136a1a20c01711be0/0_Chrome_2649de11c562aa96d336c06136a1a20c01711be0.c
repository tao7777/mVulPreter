bool ExtensionApiTest::InitializeEmbeddedTestServer() {
  if (!embedded_test_server()->InitializeAndListen())
    return false;

  if (test_config_) {
    test_config_->SetInteger(kEmbeddedTestServerPort,
                             embedded_test_server()->port());
  }
  // else SetUpOnMainThread has not been called yet. Possibly because the
  // caller needs a valid port in an overridden SetUpCommandLine method.
 
   return true;
 }
