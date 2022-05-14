ExtensionTtsPlatformImpl* ExtensionTtsController::GetPlatformImpl() {
  if (!platform_impl_)
    platform_impl_ = ExtensionTtsPlatformImpl::GetInstance();
  return platform_impl_;
}
