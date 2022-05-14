DWORD SetProcessIntegrityLevel(IntegrityLevel integrity_level) {
   if (base::win::GetVersion() < base::win::VERSION_VISTA)
     return ERROR_SUCCESS;
 
  // We don't check for an invalid level here because we'll just let it
  // fail on the SetTokenIntegrityLevel call later on.
  if (integrity_level == INTEGRITY_LEVEL_LAST) {
     return ERROR_SUCCESS;
   }
 
   HANDLE token_handle;
   if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_DEFAULT,
                           &token_handle))
    return ::GetLastError();

  base::win::ScopedHandle token(token_handle);

  return SetTokenIntegrityLevel(token.Get(), integrity_level);
}
