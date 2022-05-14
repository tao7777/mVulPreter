 void TestFlashMessageLoop::DestroyMessageLoopResourceTask(int32_t unused) {
   if (message_loop_) {
     delete message_loop_;
    message_loop_ = nullptr;
   } else {
     PP_NOTREACHED();
   }
}
