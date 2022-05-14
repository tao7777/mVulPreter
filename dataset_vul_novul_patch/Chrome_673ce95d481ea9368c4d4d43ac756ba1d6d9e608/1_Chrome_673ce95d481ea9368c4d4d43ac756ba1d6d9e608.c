void ClientDiscardableSharedMemoryManager::AllocateCompletedOnIO(
     base::SharedMemoryHandle* handle,
     base::ScopedClosureRunner closure_runner,
     mojo::ScopedSharedBufferHandle mojo_handle) {
  size_t memory_size = 0;
  bool read_only = false;
   if (!mojo_handle.is_valid())
     return;
   auto result = mojo::UnwrapSharedMemoryHandle(std::move(mojo_handle), handle,
                                               &memory_size, &read_only);
   DCHECK_EQ(result, MOJO_RESULT_OK);
 }
