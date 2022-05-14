PageHandler::PageHandler(EmulationHandler* emulation_handler)
PageHandler::PageHandler(EmulationHandler* emulation_handler,
                         bool allow_set_download_behavior)
     : DevToolsDomainHandler(Page::Metainfo::domainName),
       enabled_(false),
       screencast_enabled_(false),
      screencast_quality_(kDefaultScreenshotQuality),
      screencast_max_width_(-1),
      screencast_max_height_(-1),
      capture_every_nth_frame_(1),
      capture_retry_count_(0),
      has_compositor_frame_metadata_(false),
      session_id_(0),
      frame_counter_(0),
      frames_in_flight_(0),
      video_consumer_(nullptr),
       last_surface_size_(gfx::Size()),
       host_(nullptr),
       emulation_handler_(emulation_handler),
      allow_set_download_behavior_(allow_set_download_behavior),
       observer_(this),
       weak_factory_(this) {
   bool create_video_consumer = true;
#ifdef OS_ANDROID
  if (!CompositorImpl::IsInitialized())
    create_video_consumer = false;
#endif
  if (create_video_consumer) {
    video_consumer_ = std::make_unique<DevToolsVideoConsumer>(
        base::BindRepeating(&PageHandler::OnFrameFromVideoConsumer,
                            weak_factory_.GetWeakPtr()));
  }
  DCHECK(emulation_handler_);
}
