PlatformFileForTransit GetFileHandleForProcess(base::PlatformFile handle,
                                               base::ProcessHandle process,
                                               bool close_source_handle) {
  IPC::PlatformFileForTransit out_handle;
#if defined(OS_WIN)
   DWORD options = DUPLICATE_SAME_ACCESS;
   if (close_source_handle)
     options |= DUPLICATE_CLOSE_SOURCE;
  if (handle == INVALID_HANDLE_VALUE ||
      !::DuplicateHandle(::GetCurrentProcess(),
                          handle,
                          process,
                          &out_handle,
                         0,
                         FALSE,
                         options)) {
    out_handle = IPC::InvalidPlatformFileForTransit();
  }
#elif defined(OS_POSIX)
  int fd = close_source_handle ? handle : ::dup(handle);
  out_handle = base::FileDescriptor(fd, true);
#else
  #error Not implemented.
#endif
  return out_handle;
}
