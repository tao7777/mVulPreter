 static IMFSample* CreateSampleFromInputBuffer(
     const media::BitstreamBuffer& bitstream_buffer,
    base::ProcessHandle renderer_process,
     DWORD stream_size,
     DWORD alignment) {
  HANDLE shared_memory_handle = NULL;
  RETURN_ON_FAILURE(::DuplicateHandle(renderer_process,
                                      bitstream_buffer.handle(),
                                      base::GetCurrentProcessHandle(),
                                      &shared_memory_handle,
                                      0,
                                      FALSE,
                                      DUPLICATE_SAME_ACCESS),
                     "Duplicate handle failed", NULL);
  base::SharedMemory shm(shared_memory_handle, true);
   RETURN_ON_FAILURE(shm.Map(bitstream_buffer.size()),
                     "Failed in base::SharedMemory::Map", NULL);
 
  return CreateInputSample(reinterpret_cast<const uint8*>(shm.memory()),
                           bitstream_buffer.size(),
                           stream_size,
                           alignment);
}
