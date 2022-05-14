DWORD SetProcessIntegrityLevel(IntegrityLevel integrity_level) {
   if (base::win::GetVersion() < base::win::VERSION_VISTA)
     return ERROR_SUCCESS;
 
  const wchar_t* integrity_level_str = GetIntegrityLevelString(integrity_level);
  if (!integrity_level_str) {
     return ERROR_SUCCESS;
   }
 
  std::wstring ace_access = SDDL_NO_READ_UP;
  ace_access += SDDL_NO_WRITE_UP;
  DWORD error = SetObjectIntegrityLabel(::GetCurrentProcess(), SE_KERNEL_OBJECT,
                                        ace_access.c_str(),
                                        integrity_level_str);
  if (ERROR_SUCCESS != error)
    return error;
   HANDLE token_handle;
   if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_DEFAULT,
                           &token_handle))
    return ::GetLastError();

  base::win::ScopedHandle token(token_handle);

  return SetTokenIntegrityLevel(token.Get(), integrity_level);
}
