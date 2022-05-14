Win32StackFrameUnwinder::~Win32StackFrameUnwinder() {
  if (pending_blacklisted_module_) {
    LeafUnwindBlacklist::GetInstance()->AddModuleToBlacklist(
        pending_blacklisted_module_);
  }
}
