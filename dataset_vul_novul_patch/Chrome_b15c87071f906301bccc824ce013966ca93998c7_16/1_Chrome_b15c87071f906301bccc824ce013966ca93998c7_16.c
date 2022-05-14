DWORD WtsSessionProcessDelegate::GetExitCode() {
  if (!core_)
    return CONTROL_C_EXIT;
 
  return core_->GetExitCode();
 }
