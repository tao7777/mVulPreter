bool NaClProcessHost::StartNaClExecution() {
  NaClBrowser* nacl_browser = NaClBrowser::GetInstance();

  nacl::NaClStartParams params;
  params.validation_cache_enabled = nacl_browser->ValidationCacheIsEnabled();
  params.validation_cache_key = nacl_browser->GetValidationCacheKey();
  params.version = chrome::VersionInfo().CreateVersionString();
   params.enable_exception_handling = enable_exception_handling_;
   params.enable_debug_stub =
       CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableNaClDebug);
  params.enable_ipc_proxy = enable_ipc_proxy_;
 
   base::PlatformFile irt_file = nacl_browser->IrtFile();
   CHECK_NE(irt_file, base::kInvalidPlatformFileValue);

  const ChildProcessData& data = process_->GetData();
  for (size_t i = 0; i < internal_->sockets_for_sel_ldr.size(); i++) {
    if (!ShareHandleToSelLdr(data.handle,
                             internal_->sockets_for_sel_ldr[i], true,
                             &params.handles)) {
      return false;
    }
  }

  if (!ShareHandleToSelLdr(data.handle, irt_file, false, &params.handles))
    return false;

#if defined(OS_MACOSX)
  base::SharedMemory memory_buffer;
  base::SharedMemoryCreateOptions options;
  options.size = 1;
  options.executable = true;
  if (!memory_buffer.Create(options)) {
    DLOG(ERROR) << "Failed to allocate memory buffer";
    return false;
  }
  nacl::FileDescriptor memory_fd;
  memory_fd.fd = dup(memory_buffer.handle().fd);
  if (memory_fd.fd < 0) {
    DLOG(ERROR) << "Failed to dup() a file descriptor";
    return false;
  }
  memory_fd.auto_close = true;
  params.handles.push_back(memory_fd);
#endif

  process_->Send(new NaClProcessMsg_Start(params));

  internal_->sockets_for_sel_ldr.clear();
  return true;
 }
