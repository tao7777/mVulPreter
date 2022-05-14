 NativeStackSamplerWin::NativeStackSamplerWin(win::ScopedHandle thread_handle)
    : thread_handle_(thread_handle.Take()),
      thread_stack_base_address_(
          GetThreadEnvironmentBlock(thread_handle_.Get())->Tib.StackBase),
      stack_copy_buffer_(new unsigned char[kStackCopyBufferSize]) {
 }
