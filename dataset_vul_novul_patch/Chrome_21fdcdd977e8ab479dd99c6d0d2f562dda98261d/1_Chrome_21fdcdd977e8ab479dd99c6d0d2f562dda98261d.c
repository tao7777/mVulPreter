bool ChromotingInstance::Init(uint32_t argc,
                              const char* argn[],
                              const char* argv[]) {
  CHECK(!initialized_);
  initialized_ = true;

  VLOG(1) << "Started ChromotingInstance::Init";

  if (!media::IsMediaLibraryInitialized()) {
    LOG(ERROR) << "Media library not initialized.";
     return false;
   }
 
  net::EnableSSLServerSockets();

  context_.Start();

  scoped_refptr<FrameConsumerProxy> consumer_proxy =
      new FrameConsumerProxy(plugin_task_runner_);
  rectangle_decoder_ = new RectangleUpdateDecoder(context_.main_task_runner(),
                                                  context_.decode_task_runner(),
                                                  consumer_proxy);
  view_.reset(new PepperView(this, &context_, rectangle_decoder_.get()));
  consumer_proxy->Attach(view_->AsWeakPtr());

  return true;
}
