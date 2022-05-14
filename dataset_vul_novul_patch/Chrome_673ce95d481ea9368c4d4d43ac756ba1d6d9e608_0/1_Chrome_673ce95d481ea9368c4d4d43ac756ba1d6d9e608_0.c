bool PrintRenderFrameHelper::CopyMetafileDataToSharedMem(
     const PdfMetafileSkia& metafile,
     base::SharedMemoryHandle* shared_mem_handle) {
   uint32_t buf_size = metafile.GetDataSize();
   if (buf_size == 0)
     return false;
 
  std::unique_ptr<base::SharedMemory> shared_buf(
      content::RenderThread::Get()->HostAllocateSharedMemoryBuffer(buf_size));
  if (!shared_buf)
     return false;
 
  if (!shared_buf->Map(buf_size))
     return false;
 
  if (!metafile.GetData(shared_buf->memory(), buf_size))
     return false;
 
  *shared_mem_handle =
      base::SharedMemory::DuplicateHandle(shared_buf->handle());
   return true;
 }
