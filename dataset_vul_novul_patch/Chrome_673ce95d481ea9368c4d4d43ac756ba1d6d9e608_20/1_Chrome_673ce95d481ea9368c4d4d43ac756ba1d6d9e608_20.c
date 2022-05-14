  void UnwrapAndVerifyMojoHandle(mojo::ScopedSharedBufferHandle buffer_handle,
                                 size_t expected_size,
                                 bool expected_read_only_flag) {
     base::SharedMemoryHandle memory_handle;
     size_t memory_size = 0;
    bool read_only_flag = false;
    const MojoResult result =
        mojo::UnwrapSharedMemoryHandle(std::move(buffer_handle), &memory_handle,
                                       &memory_size, &read_only_flag);
     EXPECT_EQ(MOJO_RESULT_OK, result);
     EXPECT_EQ(expected_size, memory_size);
    EXPECT_EQ(expected_read_only_flag, read_only_flag);
   }
