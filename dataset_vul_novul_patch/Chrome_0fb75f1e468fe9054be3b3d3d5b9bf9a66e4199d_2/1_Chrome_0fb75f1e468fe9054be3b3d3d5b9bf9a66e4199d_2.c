int SuspendThreadAndRecordStack(HANDLE thread_handle, int max_stack_size,
                                const void* instruction_pointers[]) {
  Win32StackFrameUnwinder frame_unwinder;
//// ScopedSuspendThread --------------------------------------------------------
//// Suspends a thread for the lifetime of the object.
 
  if (::SuspendThread(thread_handle) == -1)
    return 0;
 
  int stack_depth = 0;
  CONTEXT thread_context = {0};
  thread_context.ContextFlags = CONTEXT_FULL;
  if (::GetThreadContext(thread_handle, &thread_context)) {
    stack_depth = RecordStack(&thread_context, max_stack_size,
                              instruction_pointers, &frame_unwinder);
  }
 
  ScopedDisablePriorityBoost disable_priority_boost(thread_handle);
  bool resume_thread_succeeded = ::ResumeThread(thread_handle) != -1;
   CHECK(resume_thread_succeeded) << "ResumeThread failed: " << GetLastError();
//// Suspends the thread with |thread_handle|, copies its stack and resumes the
//// thread, then records the stack into |instruction_pointers|. Returns the size
//// of the stack.
////
//// IMPORTANT NOTE: No allocations from the default heap may occur in the
//// ScopedSuspendThread scope, including indirectly via use of DCHECK/CHECK or
//// other logging statements. Otherwise this code can deadlock on heap locks in
//// the default heap acquired by the target thread before it was suspended. This
//// is why we pass instruction pointers as preallocated arrays.
 
  return stack_depth;
 }
