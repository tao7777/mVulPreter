   void SimulateOnBufferCreated(int buffer_id, const base::SharedMemory& shm) {
     video_capture_impl_->OnBufferCreated(
        buffer_id, mojo::WrapSharedMemoryHandle(
                       shm.GetReadOnlyHandle(), shm.mapped_size(),
                       mojo::UnwrappedSharedMemoryHandleProtection::kReadOnly));
   }
