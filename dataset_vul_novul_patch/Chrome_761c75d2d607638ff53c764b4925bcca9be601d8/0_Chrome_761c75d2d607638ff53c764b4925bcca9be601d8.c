MediaElementAudioSourceHandler::MediaElementAudioSourceHandler(
    AudioNode& node,
    HTMLMediaElement& media_element)
    : AudioHandler(kNodeTypeMediaElementAudioSource,
                   node,
                   node.context()->sampleRate()),
       media_element_(media_element),
       source_number_of_channels_(0),
       source_sample_rate_(0),
      is_origin_tainted_(false) {
   DCHECK(IsMainThread());
  AddOutput(2);

  if (Context()->GetExecutionContext()) {
    task_runner_ = Context()->GetExecutionContext()->GetTaskRunner(
        TaskType::kMediaElementEvent);
  }

  Initialize();
}
