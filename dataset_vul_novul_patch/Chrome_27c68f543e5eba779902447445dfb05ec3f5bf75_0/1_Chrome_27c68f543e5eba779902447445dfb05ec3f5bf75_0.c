bool VaapiVideoDecodeAccelerator::Initialize(media::VideoCodecProfile profile,
                                             Client* client) {
  DCHECK_EQ(message_loop_, base::MessageLoop::current());

  client_ptr_factory_.reset(new base::WeakPtrFactory<Client>(client));
  client_ = client_ptr_factory_->GetWeakPtr();

  base::AutoLock auto_lock(lock_);
  DCHECK_EQ(state_, kUninitialized);
  DVLOG(2) << "Initializing VAVDA, profile: " << profile;

#if defined(USE_X11)
  if (gfx::GetGLImplementation() != gfx::kGLImplementationDesktopGL) {
    DVLOG(1) << "HW video decode acceleration not available without "
                "DesktopGL (GLX).";
    return false;
  }
#elif defined(USE_OZONE)
  if (gfx::GetGLImplementation() != gfx::kGLImplementationEGLGLES2) {
    DVLOG(1) << "HW video decode acceleration not available without "
             << "EGLGLES2.";
    return false;
  }
#endif  // USE_X11

  vaapi_wrapper_ = VaapiWrapper::CreateForVideoCodec(
      VaapiWrapper::kDecode, profile, base::Bind(&ReportToUMA, VAAPI_ERROR));

  if (!vaapi_wrapper_.get()) {
    DVLOG(1) << "Failed initializing VAAPI for profile " << profile;
    return false;
  }

  if (profile >= media::H264PROFILE_MIN && profile <= media::H264PROFILE_MAX) {
    h264_accelerator_.reset(
        new VaapiH264Accelerator(this, vaapi_wrapper_.get()));
    decoder_.reset(new H264Decoder(h264_accelerator_.get()));
  } else if (profile >= media::VP8PROFILE_MIN &&
              profile <= media::VP8PROFILE_MAX) {
     vp8_accelerator_.reset(new VaapiVP8Accelerator(this, vaapi_wrapper_.get()));
     decoder_.reset(new VP8Decoder(vp8_accelerator_.get()));
  } else if (profile >= media::VP9PROFILE_MIN &&
             profile <= media::VP9PROFILE_MAX) {
    vp9_accelerator_.reset(new VaapiVP9Accelerator(this, vaapi_wrapper_.get()));
    decoder_.reset(new VP9Decoder(vp9_accelerator_.get()));
   } else {
     DLOG(ERROR) << "Unsupported profile " << profile;
     return false;
  }

  CHECK(decoder_thread_.Start());
  decoder_thread_task_runner_ = decoder_thread_.task_runner();

  state_ = kIdle;
  return true;
}
