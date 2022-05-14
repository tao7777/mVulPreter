   void ShutdownHost() {
    host_->Shutdown(base::Bind(&PostQuitTask, &message_loop_));
   }
