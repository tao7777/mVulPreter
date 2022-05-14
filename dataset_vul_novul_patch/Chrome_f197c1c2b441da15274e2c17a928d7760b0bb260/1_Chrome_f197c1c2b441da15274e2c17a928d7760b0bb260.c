void RenderViewTest::TearDown() {
   base::RunLoop().RunUntilIdle();
 
  ViewMsg_Close msg(view_->GetRoutingID());
  static_cast<RenderViewImpl*>(view_)->OnMessageReceived(msg);
 
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
