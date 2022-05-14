bool GpuProcessPolicy::PreSandboxHook() {
  const bool chromeos_arm_gpu = IsChromeOS() && IsArchitectureArm();
  DCHECK(!chromeos_arm_gpu);

  DCHECK(!broker_process());
  InitGpuBrokerProcess(
      GpuBrokerProcessPolicy::Create,
      std::vector<BrokerFilePermission>());  // No extra files in whitelist.

  if (IsArchitectureX86_64() || IsArchitectureI386()) {
     if (IsAcceleratedVaapiVideoEncodeEnabled() ||
         IsAcceleratedVideoDecodeEnabled()) {
       const char* I965DrvVideoPath = NULL;
 
       if (IsArchitectureX86_64()) {
         I965DrvVideoPath = "/usr/lib64/va/drivers/i965_drv_video.so";
       } else if (IsArchitectureI386()) {
         I965DrvVideoPath = "/usr/lib/va/drivers/i965_drv_video.so";
       }
 
       dlopen(I965DrvVideoPath, RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
       dlopen("libva.so.1", RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
 #if defined(USE_OZONE)
       dlopen("libva-drm.so.1", RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
#elif defined(USE_X11)
      dlopen("libva-x11.so.1", RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
#endif
    }
  }

  return true;
}
