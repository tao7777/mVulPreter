void PdfCompositorClient::Composite(
    service_manager::Connector* connector,
    base::SharedMemoryHandle handle,
    size_t data_size,
    mojom::PdfCompositor::CompositePdfCallback callback,
    scoped_refptr<base::SequencedTaskRunner> callback_task_runner) {
  DCHECK(data_size);

   if (!compositor_)
     Connect(connector);
 
  mojo::ScopedSharedBufferHandle buffer_handle = mojo::WrapSharedMemoryHandle(
      handle, data_size,
      mojo::UnwrappedSharedMemoryHandleProtection::kReadOnly);
 
   compositor_->CompositePdf(
       std::move(buffer_handle),
      base::BindOnce(&OnCompositePdf, base::Passed(&compositor_),
                     std::move(callback), callback_task_runner));
}
