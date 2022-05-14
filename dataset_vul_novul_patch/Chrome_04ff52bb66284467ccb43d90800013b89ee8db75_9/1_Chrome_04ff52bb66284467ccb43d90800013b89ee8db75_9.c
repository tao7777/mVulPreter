   void WaitForCallback() {
     if (!use_audio_thread_) {
       base::RunLoop().RunUntilIdle();
       return;
     }
    media::WaitableMessageLoopEvent event;
     audio_thread_.task_runner()->PostTaskAndReply(
         FROM_HERE, base::Bind(&base::DoNothing), event.GetClosure());
    event.RunAndWait();
    base::RunLoop().RunUntilIdle();
  }
