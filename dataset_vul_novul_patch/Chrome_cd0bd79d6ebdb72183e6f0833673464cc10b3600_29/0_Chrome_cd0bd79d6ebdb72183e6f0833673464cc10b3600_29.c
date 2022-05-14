bool BrokerDuplicateHandle(HANDLE source_handle,
                           DWORD target_process_id,
                            HANDLE* target_handle,
                            DWORD desired_access,
                            DWORD options) {
  // If our process is the target just duplicate the handle.
  if (::GetCurrentProcessId() == target_process_id) {
    return !!::DuplicateHandle(::GetCurrentProcess(), source_handle,
                               ::GetCurrentProcess(), target_handle,
                               desired_access, FALSE, options);
 
  }

  // Try the broker next
  if (g_target_services &&
      g_target_services->DuplicateHandle(source_handle, target_process_id,
                                         target_handle, desired_access,
                                         options) == SBOX_ALL_OK) {
     return true;
   }
 
  // Finally, see if we already have access to the process.
  base::win::ScopedHandle target_process;
  target_process.Set(::OpenProcess(PROCESS_DUP_HANDLE, FALSE,
                                    target_process_id));
  if (target_process.IsValid()) {
    return !!::DuplicateHandle(::GetCurrentProcess(), source_handle,
                                target_process, target_handle,
                                desired_access, FALSE, options);
  }

  return false;
 }
