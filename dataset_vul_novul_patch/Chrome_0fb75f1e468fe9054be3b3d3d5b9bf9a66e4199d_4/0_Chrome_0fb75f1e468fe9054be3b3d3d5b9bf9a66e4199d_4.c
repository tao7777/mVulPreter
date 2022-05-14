NOINLINE void TargetThread::SignalAndWaitUntilSignaled(
NOINLINE const void* TargetThread::SignalAndWaitUntilSignaled(
     WaitableEvent* thread_started_event,
     WaitableEvent* finish_event) {
  if (thread_started_event && finish_event) {
    thread_started_event->Signal();
    finish_event->Wait();
  }

  // Volatile to prevent a tail call to GetProgramCounter().
  const void* volatile program_counter = GetProgramCounter();
  return program_counter;
 }
