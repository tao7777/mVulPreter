void PPB_Buffer_Proxy::OnMsgCreate(
    PP_Instance instance,
    uint32_t size,
    HostResource* result_resource,
    ppapi::proxy::SerializedHandle* result_shm_handle) {
  result_shm_handle->set_null_shmem();
   HostDispatcher* dispatcher = HostDispatcher::GetForInstance(instance);
   if (!dispatcher)
     return;
 
   thunk::EnterResourceCreation enter(instance);
   if (enter.failed())
    return;
  PP_Resource local_buffer_resource = enter.functions()->CreateBuffer(instance,
                                                                      size);
  if (local_buffer_resource == 0)
    return;

  thunk::EnterResourceNoLock<thunk::PPB_BufferTrusted_API> trusted_buffer(
      local_buffer_resource, false);
  if (trusted_buffer.failed())
    return;
  int local_fd;
  if (trusted_buffer.object()->GetSharedMemory(&local_fd) != PP_OK)
    return;

  result_resource->SetHostResource(instance, local_buffer_resource);

  base::PlatformFile platform_file =
#if defined(OS_WIN)
      reinterpret_cast<HANDLE>(static_cast<intptr_t>(local_fd));
#elif defined(OS_POSIX)
      local_fd;
#else
  #error Not implemented.
#endif
  result_shm_handle->set_shmem(
      dispatcher->ShareHandleWithRemote(platform_file, false), size);
}
