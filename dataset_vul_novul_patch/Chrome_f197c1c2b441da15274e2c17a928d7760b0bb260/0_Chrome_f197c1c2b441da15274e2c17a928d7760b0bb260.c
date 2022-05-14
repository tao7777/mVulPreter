void RenderViewTest::TearDown() {
   base::RunLoop().RunUntilIdle();
 
  // Close the main |view_| as well as any other windows that might have been
  // opened by the test.
  CloseMessageSendingRenderViewVisitor closing_visitor;
  RenderView::ForEach(&closing_visitor);
 
   std::unique_ptr<blink::WebLeakDetector> leak_detector =
       base::WrapUnique(blink::WebLeakDetector::Create(this));

  leak_detector->PrepareForLeakDetection(view_->GetWebView()->MainFrame());

  view_ = nullptr;
  mock_process_.reset();

  RenderThreadImpl::SetRendererBlinkPlatformImplForTesting(nullptr);

  base::RunLoop().RunUntilIdle();

#if defined(OS_WIN)
  ClearDWriteFontProxySenderForTesting();
#endif

#if defined(OS_MACOSX)
  autorelease_pool_.reset();
#endif

  leak_detector->CollectGarbageAndReport();

  blink_platform_impl_.Shutdown();
  platform_->PlatformUninitialize();
  platform_.reset();
  params_.reset();
  command_line_.reset();

  test_io_thread_.reset();
  ipc_support_.reset();
}
