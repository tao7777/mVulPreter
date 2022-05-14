NOINLINE void TargetThread::SignalAndWaitUntilSignaled(
     WaitableEvent* thread_started_event,
     WaitableEvent* finish_event) {
  thread_started_event->Signal();
  volatile int x = 1;
  finish_event->Wait();
  x = 0;  // Prevent tail call to WaitableEvent::Wait().
  ALLOW_UNUSED_LOCAL(x);
 }
