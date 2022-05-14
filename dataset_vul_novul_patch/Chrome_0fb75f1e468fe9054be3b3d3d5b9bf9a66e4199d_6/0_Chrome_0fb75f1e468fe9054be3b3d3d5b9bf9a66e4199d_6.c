 void TargetThread::ThreadMain() {
   id_ = PlatformThread::CurrentId();
  if (allocation_config_ == USE_ALLOCA) {
    SignalAndWaitUntilSignaledWithAlloca(&thread_started_event_,
                                         &finish_event_);
  } else {
    SignalAndWaitUntilSignaled(&thread_started_event_, &finish_event_);
  }
 }
