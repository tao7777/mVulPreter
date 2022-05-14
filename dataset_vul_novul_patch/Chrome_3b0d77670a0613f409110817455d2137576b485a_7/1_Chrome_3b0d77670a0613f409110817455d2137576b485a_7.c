void NaClListener::OnMsgStart(const nacl::NaClStartParams& params) {
  struct NaClChromeMainArgs *args = NaClChromeMainArgsCreate();
  if (args == NULL) {
    LOG(ERROR) << "NaClChromeMainArgsCreate() failed";
     return;
   }
 
  if (params.enable_ipc_proxy) {
    IPC::ChannelHandle channel_handle =
        IPC::Channel::GenerateVerifiedChannelID("nacl");
    scoped_refptr<NaClIPCAdapter> ipc_adapter(new NaClIPCAdapter(
        channel_handle, io_thread_.message_loop_proxy()));
    args->initial_ipc_desc = ipc_adapter.get()->MakeNaClDesc();
#if defined(OS_POSIX)
    channel_handle.socket = base::FileDescriptor(
        ipc_adapter.get()->TakeClientFileDescriptor(), true);
#endif
    if (!Send(new NaClProcessHostMsg_PpapiChannelCreated(channel_handle)))
      LOG(ERROR) << "Failed to send IPC channel handle to renderer.";
  }
   std::vector<nacl::FileDescriptor> handles = params.handles;
 
 #if defined(OS_LINUX) || defined(OS_MACOSX)
  args->urandom_fd = dup(base::GetUrandomFD());
  if (args->urandom_fd < 0) {
    LOG(ERROR) << "Failed to dup() the urandom FD";
    return;
  }
  args->create_memory_object_func = CreateMemoryObject;
# if defined(OS_MACOSX)
  CHECK(handles.size() >= 1);
  g_shm_fd = nacl::ToNativeHandle(handles[handles.size() - 1]);
  handles.pop_back();
# endif
#endif

  CHECK(handles.size() >= 1);
  NaClHandle irt_handle = nacl::ToNativeHandle(handles[handles.size() - 1]);
  handles.pop_back();

#if defined(OS_WIN)
  args->irt_fd = _open_osfhandle(reinterpret_cast<intptr_t>(irt_handle),
                                 _O_RDONLY | _O_BINARY);
  if (args->irt_fd < 0) {
    LOG(ERROR) << "_open_osfhandle() failed";
    return;
  }
#else
  args->irt_fd = irt_handle;
#endif

  if (params.validation_cache_enabled) {
    CHECK_EQ(params.validation_cache_key.length(), (size_t) 64);
    args->validation_cache = CreateValidationCache(
        new BrowserValidationDBProxy(this), params.validation_cache_key,
        params.version);
  }

  CHECK(handles.size() == 1);
  args->imc_bootstrap_handle = nacl::ToNativeHandle(handles[0]);
  args->enable_exception_handling = params.enable_exception_handling;
  args->enable_debug_stub = params.enable_debug_stub;
#if defined(OS_WIN)
  args->broker_duplicate_handle_func = BrokerDuplicateHandle;
  args->attach_debug_exception_handler_func = AttachDebugExceptionHandler;
#endif
  NaClChromeMainStart(args);
  NOTREACHED();
}
