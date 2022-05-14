 static IMFSample* CreateSampleFromInputBuffer(
     const media::BitstreamBuffer& bitstream_buffer,
     DWORD stream_size,
     DWORD alignment) {
  base::SharedMemory shm(bitstream_buffer.handle(), true);
   RETURN_ON_FAILURE(shm.Map(bitstream_buffer.size()),
                     "Failed in base::SharedMemory::Map", NULL);
 
  return CreateInputSample(reinterpret_cast<const uint8*>(shm.memory()),
                           bitstream_buffer.size(),
                           stream_size,
                           alignment);
}
