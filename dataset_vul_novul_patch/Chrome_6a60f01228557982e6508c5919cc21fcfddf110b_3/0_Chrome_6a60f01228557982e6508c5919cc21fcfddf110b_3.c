 FrameImpl::FrameImpl(std::unique_ptr<content::WebContents> web_contents,
                     ContextImpl* context,
                     fidl::InterfaceRequest<chromium::web::Frame> frame_request)
    : web_contents_(std::move(web_contents)),
      context_(context),
      binding_(this, std::move(frame_request)) {
  binding_.set_error_handler([this]() { context_->DestroyFrame(this); });
  Observe(web_contents_.get());
 }
