   void CallCompositorWithSuccess(mojom::PdfCompositorPtr ptr) {
     auto handle = CreateMSKPInSharedMemory();
     ASSERT_TRUE(handle.IsValid());
    mojo::ScopedSharedBufferHandle buffer_handle = mojo::WrapSharedMemoryHandle(
        handle, handle.GetSize(),
        mojo::UnwrappedSharedMemoryHandleProtection::kReadOnly);
     ASSERT_TRUE(buffer_handle->is_valid());
     EXPECT_CALL(*this, CallbackOnSuccess(testing::_)).Times(1);
     ptr->CompositePdf(std::move(buffer_handle),
                      base::BindOnce(&PdfCompositorServiceTest::OnCallback,
                                     base::Unretained(this)));
    run_loop_->Run();
  }
