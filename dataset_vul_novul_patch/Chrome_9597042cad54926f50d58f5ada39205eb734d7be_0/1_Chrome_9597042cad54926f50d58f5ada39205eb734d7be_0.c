void GpuVideoDecodeAccelerator::Initialize(
    const media::VideoCodecProfile profile,
    IPC::Message* init_done_msg) {
  DCHECK(!video_decode_accelerator_.get());
  DCHECK(!init_done_msg_);
   DCHECK(init_done_msg);
   init_done_msg_ = init_done_msg;
 
#if defined(OS_CHROMEOS) || defined(OS_WIN)
   DCHECK(stub_ && stub_->decoder());
 #if defined(OS_WIN)
   if (base::win::GetVersion() < base::win::VERSION_WIN7) {
    NOTIMPLEMENTED() << "HW video decode acceleration not available.";
    NotifyError(media::VideoDecodeAccelerator::PLATFORM_FAILURE);
    return;
  }
   DLOG(INFO) << "Initializing DXVA HW decoder for windows.";
   DXVAVideoDecodeAccelerator* video_decoder =
       new DXVAVideoDecodeAccelerator(this);
#elif defined(OS_CHROMEOS)  // OS_WIN
#if defined(ARCH_CPU_ARMEL)
   OmxVideoDecodeAccelerator* video_decoder =
       new OmxVideoDecodeAccelerator(this);
   video_decoder->SetEglState(
       gfx::GLSurfaceEGL::GetHardwareDisplay(),
       stub_->decoder()->GetGLContext()->GetHandle());
#elif defined(ARCH_CPU_X86_FAMILY)
  VaapiVideoDecodeAccelerator* video_decoder =
      new VaapiVideoDecodeAccelerator(this);
  gfx::GLContextGLX* glx_context =
      static_cast<gfx::GLContextGLX*>(stub_->decoder()->GetGLContext());
  GLXContext glx_context_handle =
      static_cast<GLXContext>(glx_context->GetHandle());
  video_decoder->SetGlxState(glx_context->display(), glx_context_handle);
#endif  // ARCH_CPU_ARMEL
 #endif  // OS_WIN
   video_decode_accelerator_ = video_decoder;
   if (!video_decode_accelerator_->Initialize(profile))
     NotifyError(media::VideoDecodeAccelerator::PLATFORM_FAILURE);
 #else  // Update RenderViewImpl::createMediaPlayer when adding clauses.
   NOTIMPLEMENTED() << "HW video decode acceleration not available.";
   NotifyError(media::VideoDecodeAccelerator::PLATFORM_FAILURE);
#endif  // defined(OS_CHROMEOS) || defined(OS_WIN)
 }
