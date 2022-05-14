int SuspendThreadAndRecordStack(HANDLE thread_handle, int max_stack_size,
//// ScopedSuspendThread --------------------------------------------------------

//// Suspends a thread for the lifetime of the object.
class ScopedSuspendThread {
 public:
  ScopedSuspendThread(HANDLE thread_handle);
  ~ScopedSuspendThread();
 
  bool was_successful() const { return was_successful_; }
 
 private:
  HANDLE thread_handle_;
  bool was_successful_;

  DISALLOW_COPY_AND_ASSIGN(ScopedSuspendThread);
};

ScopedSuspendThread::ScopedSuspendThread(HANDLE thread_handle)
    : thread_handle_(thread_handle),
      was_successful_(::SuspendThread(thread_handle) != -1) {
}

ScopedSuspendThread::~ScopedSuspendThread() {
  if (!was_successful_)
    return;
 
  ScopedDisablePriorityBoost disable_priority_boost(thread_handle_);
  bool resume_thread_succeeded = ::ResumeThread(thread_handle_) != -1;
   CHECK(resume_thread_succeeded) << "ResumeThread failed: " << GetLastError();
}

//// Suspends the thread with |thread_handle|, copies its stack and resumes the
//// thread, then records the stack into |instruction_pointers|. Returns the size
//// of the stack.
////
//// IMPORTANT NOTE: No allocations from the default heap may occur in the
//// ScopedSuspendThread scope, including indirectly via use of DCHECK/CHECK or
//// other logging statements. Otherwise this code can deadlock on heap locks in
//// the default heap acquired by the target thread before it was suspended. This
//// is why we pass instruction pointers as preallocated arrays.
int SuspendThreadAndRecordStack(HANDLE thread_handle,
                                const void* base_address,
                                void* stack_copy_buffer,
                                size_t stack_copy_buffer_size,
                                int max_stack_size,
                                const void* instruction_pointers[]) {
  CONTEXT thread_context = {0};
  thread_context.ContextFlags = CONTEXT_FULL;
  // The stack bounds are saved to uintptr_ts for use outside
  // ScopedSuspendThread, as the thread's memory is not safe to dereference
  // beyond that point.
  const uintptr_t top = reinterpret_cast<uintptr_t>(base_address);
  uintptr_t bottom = 0u;

  {
    ScopedSuspendThread suspend_thread(thread_handle);

    if (!suspend_thread.was_successful())
      return 0;

    if (!::GetThreadContext(thread_handle, &thread_context))
      return 0;
#if defined(_WIN64)
    bottom = thread_context.Rsp;
#else
    bottom = thread_context.Esp;
#endif

    if ((top - bottom) > stack_copy_buffer_size)
      return 0;

    std::memcpy(stack_copy_buffer, reinterpret_cast<const void*>(bottom),
                top - bottom);
  }

  RewritePointersToStackMemory(top, bottom, &thread_context, stack_copy_buffer);
 
  return RecordStack(&thread_context, max_stack_size, instruction_pointers);
 }
