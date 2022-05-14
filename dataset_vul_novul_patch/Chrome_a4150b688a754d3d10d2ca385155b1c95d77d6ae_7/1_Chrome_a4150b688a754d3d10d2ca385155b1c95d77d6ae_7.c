WebGLRenderingContextBase::WebGLRenderingContextBase(
    CanvasRenderingContextHost* host,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    std::unique_ptr<WebGraphicsContext3DProvider> context_provider,
    bool using_gpu_compositing,
    const CanvasContextCreationAttributesCore& requested_attributes,
    Platform::ContextType context_type)
    : CanvasRenderingContext(host, requested_attributes),
      context_group_(MakeGarbageCollected<WebGLContextGroup>()),
      dispatch_context_lost_event_timer_(
          task_runner,
          this,
          &WebGLRenderingContextBase::DispatchContextLostEvent),
      restore_timer_(task_runner,
                     this,
                      &WebGLRenderingContextBase::MaybeRestoreContext),
       task_runner_(task_runner),
       num_gl_errors_to_console_allowed_(kMaxGLErrorsAllowedToConsole),
      context_type_(context_type) {
   DCHECK(context_provider);
 
  xr_compatible_ = requested_attributes.xr_compatible;

  context_group_->AddContext(this);

  max_viewport_dims_[0] = max_viewport_dims_[1] = 0;
  context_provider->ContextGL()->GetIntegerv(GL_MAX_VIEWPORT_DIMS,
                                             max_viewport_dims_);
  InitializeWebGLContextLimits(context_provider.get());

  scoped_refptr<DrawingBuffer> buffer;
  buffer =
      CreateDrawingBuffer(std::move(context_provider), using_gpu_compositing);
  if (!buffer) {
    context_lost_mode_ = kSyntheticLostContext;
    return;
  }

  drawing_buffer_ = std::move(buffer);
  GetDrawingBuffer()->Bind(GL_FRAMEBUFFER);
  SetupFlags();

  String disabled_webgl_extensions(GetDrawingBuffer()
                                       ->ContextProvider()
                                       ->GetGpuFeatureInfo()
                                       .disabled_webgl_extensions.c_str());
  Vector<String> disabled_extension_list;
  disabled_webgl_extensions.Split(' ', disabled_extension_list);
  for (const auto& entry : disabled_extension_list) {
    disabled_extensions_.insert(entry);
  }

#define ADD_VALUES_TO_SET(set, values)              \
  for (size_t i = 0; i < base::size(values); ++i) { \
    set.insert(values[i]);                          \
  }

  ADD_VALUES_TO_SET(supported_internal_formats_, kSupportedFormatsES2);
  ADD_VALUES_TO_SET(supported_tex_image_source_internal_formats_,
                    kSupportedFormatsES2);
  ADD_VALUES_TO_SET(supported_internal_formats_copy_tex_image_,
                    kSupportedFormatsES2);
  ADD_VALUES_TO_SET(supported_formats_, kSupportedFormatsES2);
  ADD_VALUES_TO_SET(supported_tex_image_source_formats_, kSupportedFormatsES2);
  ADD_VALUES_TO_SET(supported_types_, kSupportedTypesES2);
  ADD_VALUES_TO_SET(supported_tex_image_source_types_, kSupportedTypesES2);
}
