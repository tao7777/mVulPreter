 void ContextImpl::CreateFrame(
    fidl::InterfaceHandle<chromium::web::FrameObserver> observer,
     fidl::InterfaceRequest<chromium::web::Frame> frame_request) {
   auto web_contents = content::WebContents::Create(
       content::WebContents::CreateParams(browser_context_, nullptr));
  frame_bindings_.AddBinding(
      std::make_unique<FrameImpl>(std::move(web_contents), observer.Bind()),
      std::move(frame_request));
 }
