 void TestFlashMessageLoop::DestroyMessageLoopResourceTask(int32_t unused) {
   if (message_loop_) {
     delete message_loop_;
    message_loop_ = NULL;
   } else {
     PP_NOTREACHED();
   }
}
