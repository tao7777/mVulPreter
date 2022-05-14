 std::string TestFlashMessageLoop::TestBasics() {
  message_loop_ = new pp::flash::MessageLoop(instance_);

  pp::CompletionCallback callback = callback_factory_.NewCallback(
      &TestFlashMessageLoop::QuitMessageLoopTask);
  pp::Module::Get()->core()->CallOnMainThread(0, callback);
  int32_t result = message_loop_->Run();
 
   ASSERT_TRUE(message_loop_);
   delete message_loop_;
  message_loop_ = nullptr;
 
   ASSERT_EQ(PP_OK, result);
   PASS();
}
