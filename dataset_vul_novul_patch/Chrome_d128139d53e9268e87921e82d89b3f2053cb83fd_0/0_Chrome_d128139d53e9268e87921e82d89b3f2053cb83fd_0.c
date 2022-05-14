void GLManager::InitializeWithWorkaroundsImpl(
    const GLManager::Options& options,
    const GpuDriverBugWorkarounds& workarounds) {
  const SharedMemoryLimits limits;
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  DCHECK(!command_line.HasSwitch(switches::kDisableGLExtensions));
   InitializeGpuPreferencesForTestingFromCommandLine(command_line,
                                                     &gpu_preferences_);
 
  context_type_ = options.context_type;
   if (options.share_mailbox_manager) {
     mailbox_manager_ = options.share_mailbox_manager->mailbox_manager();
   } else if (options.share_group_manager) {
    mailbox_manager_ = options.share_group_manager->mailbox_manager();
  } else {
    mailbox_manager_ = &owned_mailbox_manager_;
  }

  gl::GLShareGroup* share_group = NULL;
  if (options.share_group_manager) {
    share_group = options.share_group_manager->share_group();
  } else if (options.share_mailbox_manager) {
    share_group = options.share_mailbox_manager->share_group();
  }

  gles2::ContextGroup* context_group = NULL;
  scoped_refptr<gles2::ShareGroup> client_share_group;
  if (options.share_group_manager) {
    context_group = options.share_group_manager->decoder_->GetContextGroup();
    client_share_group =
      options.share_group_manager->gles2_implementation()->share_group();
  }

  gl::GLContext* real_gl_context = NULL;
  if (options.virtual_manager &&
      !gpu_preferences_.use_passthrough_cmd_decoder) {
    real_gl_context = options.virtual_manager->context();
  }

  share_group_ = share_group ? share_group : new gl::GLShareGroup;

  ContextCreationAttribs attribs;
  attribs.red_size = 8;
  attribs.green_size = 8;
  attribs.blue_size = 8;
  attribs.alpha_size = 8;
  attribs.depth_size = 16;
  attribs.stencil_size = 8;
  attribs.context_type = options.context_type;
  attribs.samples = options.multisampled ? 4 : 0;
  attribs.sample_buffers = options.multisampled ? 1 : 0;
  attribs.alpha_size = options.backbuffer_alpha ? 8 : 0;
  attribs.should_use_native_gmb_for_backbuffer =
      options.image_factory != nullptr;
  attribs.offscreen_framebuffer_size = options.size;
  attribs.buffer_preserved = options.preserve_backbuffer;
  attribs.bind_generates_resource = options.bind_generates_resource;
  translator_cache_ =
      std::make_unique<gles2::ShaderTranslatorCache>(gpu_preferences_);

  if (!context_group) {
    scoped_refptr<gles2::FeatureInfo> feature_info =
        new gles2::FeatureInfo(workarounds);
    context_group = new gles2::ContextGroup(
        gpu_preferences_, true, mailbox_manager_, nullptr /* memory_tracker */,
        translator_cache_.get(), &completeness_cache_, feature_info,
        options.bind_generates_resource, &image_manager_, options.image_factory,
        nullptr /* progress_reporter */, GpuFeatureInfo(),
        &discardable_manager_);
  }

  command_buffer_.reset(new CommandBufferCheckLostContext(
      context_group->transfer_buffer_manager(), options.sync_point_manager,
      options.context_lost_allowed));

  decoder_.reset(::gpu::gles2::GLES2Decoder::Create(
      command_buffer_.get(), command_buffer_->service(), &outputter_,
      context_group));
  if (options.force_shader_name_hashing) {
    decoder_->SetForceShaderNameHashingForTest(true);
  }

  command_buffer_->set_handler(decoder_.get());

  surface_ = gl::init::CreateOffscreenGLSurface(gfx::Size());
  ASSERT_TRUE(surface_.get() != NULL) << "could not create offscreen surface";

  if (base_context_) {
    context_ = scoped_refptr<gl::GLContext>(new gpu::GLContextVirtual(
        share_group_.get(), base_context_->get(), decoder_->AsWeakPtr()));
    ASSERT_TRUE(context_->Initialize(
        surface_.get(), GenerateGLContextAttribs(attribs, context_group)));
  } else {
    if (real_gl_context) {
      context_ = scoped_refptr<gl::GLContext>(new gpu::GLContextVirtual(
          share_group_.get(), real_gl_context, decoder_->AsWeakPtr()));
      ASSERT_TRUE(context_->Initialize(
          surface_.get(), GenerateGLContextAttribs(attribs, context_group)));
    } else {
      context_ = gl::init::CreateGLContext(
          share_group_.get(), surface_.get(),
          GenerateGLContextAttribs(attribs, context_group));
      g_gpu_feature_info.ApplyToGLContext(context_.get());
    }
  }
  ASSERT_TRUE(context_.get() != NULL) << "could not create GL context";

  ASSERT_TRUE(context_->MakeCurrent(surface_.get()));

  auto result =
      decoder_->Initialize(surface_.get(), context_.get(), true,
                           ::gpu::gles2::DisallowedFeatures(), attribs);
  if (result != gpu::ContextResult::kSuccess)
    return;
  capabilities_ = decoder_->GetCapabilities();

  gles2_helper_.reset(new gles2::GLES2CmdHelper(command_buffer_.get()));
  ASSERT_EQ(gles2_helper_->Initialize(limits.command_buffer_size),
            gpu::ContextResult::kSuccess);

  transfer_buffer_.reset(new TransferBuffer(gles2_helper_.get()));

  const bool support_client_side_arrays = true;
  gles2_implementation_.reset(new gles2::GLES2Implementation(
      gles2_helper_.get(), std::move(client_share_group),
      transfer_buffer_.get(), options.bind_generates_resource,
      options.lose_context_when_out_of_memory, support_client_side_arrays,
      this));

  ASSERT_EQ(gles2_implementation_->Initialize(limits),
            gpu::ContextResult::kSuccess)
      << "Could not init GLES2Implementation";

  MakeCurrent();
}
