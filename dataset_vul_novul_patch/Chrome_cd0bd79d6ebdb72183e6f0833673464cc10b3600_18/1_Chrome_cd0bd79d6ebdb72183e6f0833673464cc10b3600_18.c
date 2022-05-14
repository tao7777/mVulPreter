 GpuChannel::~GpuChannel() {
#if defined(OS_WIN)
  if (renderer_process_)
    CloseHandle(renderer_process_);
#endif
 }
