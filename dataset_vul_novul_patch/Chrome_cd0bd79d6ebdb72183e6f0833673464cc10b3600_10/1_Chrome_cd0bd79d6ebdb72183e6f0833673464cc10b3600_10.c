int32 CommandBufferProxyImpl::RegisterTransferBuffer(
    base::SharedMemory* shared_memory,
    size_t size,
    int32 id_request) {
   if (last_state_.error != gpu::error::kNoError)
     return -1;
 
   int32 id;
   if (!Send(new GpuCommandBufferMsg_RegisterTransferBuffer(
       route_id_,
      shared_memory->handle(),  // Returns FileDescriptor with auto_close off.
       size,
       id_request,
       &id))) {
    return -1;
  }

  return id;
}
