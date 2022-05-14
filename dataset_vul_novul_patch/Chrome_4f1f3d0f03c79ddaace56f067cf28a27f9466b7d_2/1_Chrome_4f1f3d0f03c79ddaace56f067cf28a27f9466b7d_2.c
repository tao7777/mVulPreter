NTSTATUS NtCreateFileInTarget(HANDLE* target_file_handle,
                              ACCESS_MASK desired_access,
                              OBJECT_ATTRIBUTES* obj_attributes,
                              IO_STATUS_BLOCK* io_status_block,
                              ULONG file_attributes,
                              ULONG share_access,
                              ULONG create_disposition,
                              ULONG create_options,
                              PVOID ea_buffer,
                              ULONG ea_lenght,
                              HANDLE target_process) {
  NtCreateFileFunction NtCreateFile = NULL;
  ResolveNTFunctionPtr("NtCreateFile", &NtCreateFile);

  HANDLE local_handle = INVALID_HANDLE_VALUE;
  NTSTATUS status = NtCreateFile(&local_handle, desired_access, obj_attributes,
                                 io_status_block, NULL, file_attributes,
                                 share_access, create_disposition,
                                 create_options, ea_buffer, ea_lenght);
  if (!NT_SUCCESS(status)) {
     return status;
   }
 
   if (!::DuplicateHandle(::GetCurrentProcess(), local_handle,
                          target_process, target_file_handle, 0, FALSE,
                          DUPLICATE_CLOSE_SOURCE | DUPLICATE_SAME_ACCESS)) {
    ::CloseHandle(local_handle);
    return STATUS_ACCESS_DENIED;
  }
   return STATUS_SUCCESS;
 }
