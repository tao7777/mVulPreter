   void Wait() {
    run_loop_->Run();
    run_loop_ = std::make_unique<base::RunLoop>();
   }
