 void BrowserMainParts::PostDestroyThreads() {
   device_client_.reset();
 
   display::Screen::SetScreenInstance(nullptr);
  gpu::oxide_shim::SetGLShareGroup(nullptr);
  gl_share_context_ = nullptr;

#if defined(OS_LINUX)
  gpu::SetGpuInfoCollectorOxideLinux(nullptr);
#endif
}
