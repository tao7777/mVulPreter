 bool CreateIpcChannel(
     const std::string& channel_name,
     const std::string& pipe_security_descriptor,
    base::win::ScopedHandle* pipe_out) {
   SECURITY_ATTRIBUTES security_attributes;
   security_attributes.nLength = sizeof(security_attributes);
  security_attributes.bInheritHandle = FALSE;

  ULONG security_descriptor_length = 0;
  if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
          UTF8ToUTF16(pipe_security_descriptor).c_str(),
          SDDL_REVISION_1,
          reinterpret_cast<PSECURITY_DESCRIPTOR*>(
              &security_attributes.lpSecurityDescriptor),
          &security_descriptor_length)) {
    LOG_GETLASTERROR(ERROR) <<
        "Failed to create a security descriptor for the Chromoting IPC channel";
    return false;
  }

  std::string pipe_name(kChromePipeNamePrefix);
  pipe_name.append(channel_name);

  base::win::ScopedHandle pipe;
  pipe.Set(CreateNamedPipe(
      UTF8ToUTF16(pipe_name).c_str(),
      PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE,
      PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
      1,
      IPC::Channel::kReadBufferSize,
      IPC::Channel::kReadBufferSize,
      5000,
      &security_attributes));
  if (!pipe.IsValid()) {
    LOG_GETLASTERROR(ERROR) <<
        "Failed to create the server end of the Chromoting IPC channel";
    LocalFree(security_attributes.lpSecurityDescriptor);
    return false;
  }
 
   LocalFree(security_attributes.lpSecurityDescriptor);
 
  *pipe_out = pipe.Pass();
   return true;
 }
