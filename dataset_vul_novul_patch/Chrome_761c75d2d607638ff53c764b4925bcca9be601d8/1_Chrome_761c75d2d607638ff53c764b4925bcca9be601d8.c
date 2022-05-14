MediaElementAudioSourceHandler::MediaElementAudioSourceHandler(
    AudioNode& node,
    HTMLMediaElement& media_element)
    : AudioHandler(kNodeTypeMediaElementAudioSource,
                   node,
                   node.context()->sampleRate()),
       media_element_(media_element),
       source_number_of_channels_(0),
       source_sample_rate_(0),
      passes_current_src_cors_access_check_(
          PassesCurrentSrcCORSAccessCheck(media_element.currentSrc())),
      maybe_print_cors_message_(!passes_current_src_cors_access_check_),
      current_src_string_(media_element.currentSrc().GetString()) {
   DCHECK(IsMainThread());
  AddOutput(2);

  if (Context()->GetExecutionContext()) {
    task_runner_ = Context()->GetExecutionContext()->GetTaskRunner(
        TaskType::kMediaElementEvent);
  }

  Initialize();
}
