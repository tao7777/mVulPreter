bool BrokerDuplicateHandle(HANDLE source_handle,
                           DWORD target_process_id,
                            HANDLE* target_handle,
                            DWORD desired_access,
                            DWORD options) {
  if (!g_target_services) {
    base::win::ScopedHandle target_process(::OpenProcess(PROCESS_DUP_HANDLE,
                                                         FALSE,
                                                         target_process_id));
    if (!target_process.IsValid())
      return false;
    if (!::DuplicateHandle(::GetCurrentProcess(), source_handle,
                           target_process, target_handle,
                           desired_access, FALSE,
                           options)) {
      return false;
    }
 
     return true;
   }
 
  ResultCode result = g_target_services->DuplicateHandle(source_handle,
                                                         target_process_id,
                                                         target_handle,
                                                         desired_access,
                                                         options);
  return SBOX_ALL_OK == result;
 }
