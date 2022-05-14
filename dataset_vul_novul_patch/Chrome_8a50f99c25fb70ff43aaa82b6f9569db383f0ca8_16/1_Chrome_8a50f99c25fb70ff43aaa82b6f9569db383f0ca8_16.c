   virtual void TearDown() {
    message_loop_.RunAllPending();
    client_.Stop();
   }
