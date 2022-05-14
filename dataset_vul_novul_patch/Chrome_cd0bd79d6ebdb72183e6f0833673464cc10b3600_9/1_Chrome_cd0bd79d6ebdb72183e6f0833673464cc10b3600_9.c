int32 CommandBufferProxyImpl::CreateTransferBuffer(
    size_t size, int32 id_request) {
  if (last_state_.error != gpu::error::kNoError)
    return -1;

  scoped_ptr<base::SharedMemory> shm(
      channel_->factory()->AllocateSharedMemory(size));
  if (!shm.get())
     return -1;
 
   base::SharedMemoryHandle handle = shm->handle();
#if defined(OS_POSIX)
   DCHECK(!handle.auto_close);
 #endif
 
  int32 id;
  if (!Send(new GpuCommandBufferMsg_RegisterTransferBuffer(route_id_,
                                                           handle,
                                                           size,
                                                           id_request,
                                                           &id))) {
    return -1;
  }

  return id;
}
