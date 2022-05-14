int32 CommandBufferProxyImpl::RegisterTransferBuffer(
    base::SharedMemory* shared_memory,
    size_t size,
    int32 id_request) {
   if (last_state_.error != gpu::error::kNoError)
     return -1;
 
  // Returns FileDescriptor with auto_close off.
  base::SharedMemoryHandle handle = shared_memory->handle();
#if defined(OS_WIN)
  // Windows needs to explicitly duplicate the handle out to another process.
  if (!sandbox::BrokerDuplicateHandle(handle, channel_->gpu_pid(),
                                      &handle, FILE_MAP_WRITE, 0)) {
    return -1;
  }
#endif

   int32 id;
   if (!Send(new GpuCommandBufferMsg_RegisterTransferBuffer(
       route_id_,
      handle,
       size,
       id_request,
       &id))) {
    return -1;
  }

  return id;
}
