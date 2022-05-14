   void Wait() {
    message_loop_runner_->Run();
    message_loop_runner_ = new MessageLoopRunner;
   }
