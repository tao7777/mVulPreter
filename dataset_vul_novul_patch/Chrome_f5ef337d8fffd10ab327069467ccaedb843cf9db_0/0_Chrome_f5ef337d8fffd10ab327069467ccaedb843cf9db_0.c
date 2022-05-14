MediaRecorder::MediaRecorder(ExecutionContext* context,
                             MediaStream* stream,
                             const MediaRecorderOptions* options,
                             ExceptionState& exception_state)
    : PausableObject(context),
      stream_(stream),
      mime_type_(options->hasMimeType() ? options->mimeType()
                                        : kDefaultMimeType),
      stopped_(true),
      audio_bits_per_second_(0),
      video_bits_per_second_(0),
      state_(State::kInactive),
      dispatch_scheduled_event_runner_(AsyncMethodRunner<MediaRecorder>::Create(
           this,
           &MediaRecorder::DispatchScheduledEvent,
           context->GetTaskRunner(TaskType::kDOMManipulation))) {
  if (context->IsContextDestroyed()) {
    exception_state.ThrowDOMException(DOMExceptionCode::kNotAllowedError,
                                      "Execution context is detached.");
    return;
  }
 
  DCHECK(stream_->getTracks().size());
   recorder_handler_ = Platform::Current()->CreateMediaRecorderHandler(
       context->GetTaskRunner(TaskType::kInternalMediaRealTime));
   DCHECK(recorder_handler_);

  if (!recorder_handler_) {
    exception_state.ThrowDOMException(
        DOMExceptionCode::kNotSupportedError,
        "No MediaRecorder handler can be created.");
    return;
  }

  AllocateVideoAndAudioBitrates(exception_state, context, options, stream,
                                &audio_bits_per_second_,
                                &video_bits_per_second_);

  const ContentType content_type(mime_type_);
  if (!recorder_handler_->Initialize(
          this, stream->Descriptor(), content_type.GetType(),
          content_type.Parameter("codecs"), audio_bits_per_second_,
          video_bits_per_second_)) {
    exception_state.ThrowDOMException(
        DOMExceptionCode::kNotSupportedError,
        "Failed to initialize native MediaRecorder the type provided (" +
            mime_type_ + ") is not supported.");
    return;
  }
  if (options->mimeType().IsEmpty()) {
    const String actual_mime_type = recorder_handler_->ActualMimeType();
    if (!actual_mime_type.IsEmpty())
      mime_type_ = actual_mime_type;
  }
  stopped_ = false;
}
