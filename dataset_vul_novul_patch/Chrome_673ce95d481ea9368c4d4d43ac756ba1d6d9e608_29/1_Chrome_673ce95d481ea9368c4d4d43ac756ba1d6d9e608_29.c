  void Initialized(mojo::ScopedSharedBufferHandle shared_buffer,
                   mojo::ScopedHandle socket_handle) {
    ASSERT_TRUE(shared_buffer.is_valid());
    ASSERT_TRUE(socket_handle.is_valid());

    base::PlatformFile fd;
    mojo::UnwrapPlatformFile(std::move(socket_handle), &fd);
    socket_ = std::make_unique<base::CancelableSyncSocket>(fd);
    EXPECT_NE(socket_->handle(), base::CancelableSyncSocket::kInvalidHandle);
 
     size_t memory_length;
     base::SharedMemoryHandle shmem_handle;
    bool read_only;
     EXPECT_EQ(
         mojo::UnwrapSharedMemoryHandle(std::move(shared_buffer), &shmem_handle,
                                       &memory_length, &read_only),
         MOJO_RESULT_OK);
    EXPECT_FALSE(read_only);
    buffer_ = std::make_unique<base::SharedMemory>(shmem_handle, read_only);
 
     GotNotification();
   }
