 void EnableHighDPISupport() {
   if (IsHighDPIEnabled() &&
      !SetProcessDpiAwarenessWrapper(PROCESS_SYSTEM_DPI_AWARE)) {
    SetProcessDPIAwareWrapper();
   }
 }
