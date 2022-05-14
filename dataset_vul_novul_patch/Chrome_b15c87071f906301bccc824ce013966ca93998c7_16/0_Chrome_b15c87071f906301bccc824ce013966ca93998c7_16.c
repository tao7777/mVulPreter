DWORD WtsSessionProcessDelegate::GetExitCode() {
DWORD WtsSessionProcessDelegate::GetProcessId() const {
  if (core_)
    return 0;

  return core_->GetProcessId();
}

bool WtsSessionProcessDelegate::IsPermanentError(int failure_count) const {
  if (core_)
    return false;
 
  return core_->IsPermanentError(failure_count);
 }
