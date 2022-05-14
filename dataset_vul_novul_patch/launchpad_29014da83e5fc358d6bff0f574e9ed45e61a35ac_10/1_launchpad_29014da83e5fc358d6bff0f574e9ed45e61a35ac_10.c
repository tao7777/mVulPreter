 void BrowserMainParts::PostDestroyThreads() {
  if (BrowserProcessMain::GetInstance()->GetProcessModel() ==
      PROCESS_MODEL_SINGLE_PROCESS) {
    BrowserContext::AssertNoContextsExist();
  }
   device_client_.reset();
 
   display::Screen::SetScreenInstance(nullptr);
  gpu::oxide_shim::SetGLShareGroup(nullptr);
  gl_share_context_ = nullptr;

#if defined(OS_LINUX)
  gpu::SetGpuInfoCollectorOxideLinux(nullptr);
#endif
}
