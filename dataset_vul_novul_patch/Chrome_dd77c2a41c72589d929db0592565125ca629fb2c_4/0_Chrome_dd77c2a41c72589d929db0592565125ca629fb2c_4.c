std::string TestFlashMessageLoop::TestRunWithoutQuit() {
  message_loop_ = new pp::flash::MessageLoop(instance_);

  pp::CompletionCallback callback = callback_factory_.NewCallback(
      &TestFlashMessageLoop::DestroyMessageLoopResourceTask);
  pp::Module::Get()->core()->CallOnMainThread(0, callback);
  int32_t result = message_loop_->Run();
 
   if (message_loop_) {
     delete message_loop_;
    message_loop_ = nullptr;
     ASSERT_TRUE(false);
   }
 
   ASSERT_EQ(PP_ERROR_ABORTED, result);
   PASS();
 }
