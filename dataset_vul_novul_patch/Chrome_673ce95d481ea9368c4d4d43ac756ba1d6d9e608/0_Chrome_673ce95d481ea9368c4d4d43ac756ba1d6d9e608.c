void ClientDiscardableSharedMemoryManager::AllocateCompletedOnIO(
     base::SharedMemoryHandle* handle,
     base::ScopedClosureRunner closure_runner,
     mojo::ScopedSharedBufferHandle mojo_handle) {
   if (!mojo_handle.is_valid())
     return;
   auto result = mojo::UnwrapSharedMemoryHandle(std::move(mojo_handle), handle,
                                               nullptr, nullptr);
   DCHECK_EQ(result, MOJO_RESULT_OK);
 }
