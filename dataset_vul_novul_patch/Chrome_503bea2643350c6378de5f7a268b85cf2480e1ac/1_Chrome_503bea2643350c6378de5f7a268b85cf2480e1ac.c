void AudioInputRendererHost::OnCreateStream(
    int stream_id, const media::AudioParameters& params,
    const std::string& device_id, bool automatic_gain_control) {
   VLOG(1) << "AudioInputRendererHost::OnCreateStream(stream_id="
           << stream_id << ")";
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  DCHECK(LookupById(stream_id) == NULL);
 
   media::AudioParameters audio_params(params);
 
  if (media_stream_manager_->audio_input_device_manager()->
      ShouldUseFakeDevice()) {
    audio_params.Reset(media::AudioParameters::AUDIO_FAKE,
                       params.channel_layout(), params.sample_rate(),
                       params.bits_per_sample(), params.frames_per_buffer());
  } else if (WebContentsCaptureUtil::IsWebContentsDeviceId(device_id)) {
    audio_params.Reset(media::AudioParameters::AUDIO_VIRTUAL,
                       params.channel_layout(), params.sample_rate(),
                        params.bits_per_sample(), params.frames_per_buffer());
   }
 
  DCHECK_GT(audio_params.frames_per_buffer(), 0);
   uint32 buffer_size = audio_params.GetBytesPerBuffer();
 
  scoped_ptr<AudioEntry> entry(new AudioEntry());

  uint32 mem_size = sizeof(media::AudioInputBufferParameters) + buffer_size;

  if (!entry->shared_memory.CreateAndMapAnonymous(mem_size)) {
    SendErrorMessage(stream_id);
    return;
  }

  scoped_ptr<AudioInputSyncWriter> writer(
      new AudioInputSyncWriter(&entry->shared_memory));

  if (!writer->Init()) {
    SendErrorMessage(stream_id);
    return;
  }

  entry->writer.reset(writer.release());
  entry->controller = media::AudioInputController::CreateLowLatency(
      audio_manager_,
      this,
      audio_params,
      device_id,
      entry->writer.get());

  if (!entry->controller) {
    SendErrorMessage(stream_id);
    return;
  }

  if (params.format() == media::AudioParameters::AUDIO_PCM_LOW_LATENCY)
    entry->controller->SetAutomaticGainControl(automatic_gain_control);

  entry->stream_id = stream_id;

  audio_entries_.insert(std::make_pair(stream_id, entry.release()));
}
