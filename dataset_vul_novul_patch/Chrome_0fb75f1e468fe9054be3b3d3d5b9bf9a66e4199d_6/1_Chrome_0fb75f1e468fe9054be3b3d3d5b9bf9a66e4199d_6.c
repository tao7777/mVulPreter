 void TargetThread::ThreadMain() {
   id_ = PlatformThread::CurrentId();
  SignalAndWaitUntilSignaled(&thread_started_event_, &finish_event_);
 }
