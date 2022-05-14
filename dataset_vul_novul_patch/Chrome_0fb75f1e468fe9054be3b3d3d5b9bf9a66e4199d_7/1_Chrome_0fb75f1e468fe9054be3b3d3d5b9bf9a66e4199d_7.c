void WithTargetThread(Function function) {
  TargetThread target_thread;
   PlatformThreadHandle target_thread_handle;
   EXPECT_TRUE(PlatformThread::Create(0, &target_thread, &target_thread_handle));
 
  target_thread.WaitForThreadStart();

  function(target_thread.id());

  target_thread.SignalThreadToFinish();

   PlatformThread::Join(target_thread_handle);
 }
