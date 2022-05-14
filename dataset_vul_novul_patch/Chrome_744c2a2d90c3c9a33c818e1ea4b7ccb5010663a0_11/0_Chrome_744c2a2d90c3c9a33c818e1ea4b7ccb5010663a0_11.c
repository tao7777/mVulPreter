 void RenderViewImpl::OnAllowBindings(int enabled_bindings_flags) {
   enabled_bindings_ |= enabled_bindings_flags;

  // Keep track of the total bindings accumulated in this process.
  RenderProcess::current()->AddBindings(enabled_bindings_flags);
 }
