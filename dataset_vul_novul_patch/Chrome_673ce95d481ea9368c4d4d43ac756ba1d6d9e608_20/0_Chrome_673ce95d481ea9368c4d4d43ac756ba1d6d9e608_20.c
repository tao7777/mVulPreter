  void UnwrapAndVerifyMojoHandle(mojo::ScopedSharedBufferHandle buffer_handle,
  void UnwrapAndVerifyMojoHandle(
      mojo::ScopedSharedBufferHandle buffer_handle,
      size_t expected_size,
      mojo::UnwrappedSharedMemoryHandleProtection expected_protection) {
     base::SharedMemoryHandle memory_handle;
     size_t memory_size = 0;
    mojo::UnwrappedSharedMemoryHandleProtection protection;
    const MojoResult result = mojo::UnwrapSharedMemoryHandle(
        std::move(buffer_handle), &memory_handle, &memory_size, &protection);
     EXPECT_EQ(MOJO_RESULT_OK, result);
     EXPECT_EQ(expected_size, memory_size);
    EXPECT_EQ(expected_protection, protection);
   }
