bool PrintRenderFrameHelper::CopyMetafileDataToSharedMem(
bool PrintRenderFrameHelper::CopyMetafileDataToReadOnlySharedMem(
     const PdfMetafileSkia& metafile,
     base::SharedMemoryHandle* shared_mem_handle) {
   uint32_t buf_size = metafile.GetDataSize();
   if (buf_size == 0)
     return false;
 
  mojo::ScopedSharedBufferHandle buffer =
      mojo::SharedBufferHandle::Create(buf_size);
  if (!buffer.is_valid())
     return false;
 
  mojo::ScopedSharedBufferMapping mapping = buffer->Map(buf_size);
  if (!mapping)
     return false;
 
  if (!metafile.GetData(mapping.get(), buf_size))
     return false;
 
  MojoResult result = mojo::UnwrapSharedMemoryHandle(
      buffer->Clone(mojo::SharedBufferHandle::AccessMode::READ_ONLY),
      shared_mem_handle, nullptr, nullptr);
  DCHECK_EQ(MOJO_RESULT_OK, result);
   return true;
 }
