 void TestFlashMessageLoop::RunTests(const std::string& filter) {
   RUN_TEST(Basics, filter);
   RUN_TEST(RunWithoutQuit, filter);
  RUN_TEST(SuspendScriptCallbackWhileRunning, filter);
}

void TestFlashMessageLoop::DidRunScriptCallback() {
  // Script callbacks are not supposed to run while the Flash message loop is
  // running.
  if (message_loop_)
    suspend_script_callback_result_ = false;
}

pp::deprecated::ScriptableObject* TestFlashMessageLoop::CreateTestObject() {
  if (!instance_so_)
    instance_so_ = new InstanceSO(this);
  return instance_so_;
 }
