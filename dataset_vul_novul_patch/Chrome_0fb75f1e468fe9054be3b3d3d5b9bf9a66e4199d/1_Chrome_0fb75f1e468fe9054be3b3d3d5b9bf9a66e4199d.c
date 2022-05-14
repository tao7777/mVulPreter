 NativeStackSamplerWin::NativeStackSamplerWin(win::ScopedHandle thread_handle)
    : thread_handle_(thread_handle.Take()) {
 }
