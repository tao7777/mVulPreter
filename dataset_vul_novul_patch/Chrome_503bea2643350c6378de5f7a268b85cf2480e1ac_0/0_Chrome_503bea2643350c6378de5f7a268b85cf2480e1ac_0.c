 void AudioRendererHost::OnCreateStream(
     int stream_id, const media::AudioParameters& params, int input_channels) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  // media::AudioParameters is validated in the deserializer.
  if (input_channels < 0 ||
      input_channels > media::limits::kMaxChannels ||
      LookupById(stream_id) != NULL) {
    SendErrorMessage(stream_id);
    return;
  }
 
   media::AudioParameters audio_params(params);
 
   int output_memory_size = AudioBus::CalculateMemorySize(audio_params);
 
   int frames = audio_params.frames_per_buffer();
   int input_memory_size =
       AudioBus::CalculateMemorySize(input_channels, frames);
 
   scoped_ptr<AudioEntry> entry(new AudioEntry());
 
  uint32 io_buffer_size = output_memory_size + input_memory_size;

  uint32 shared_memory_size =
      media::TotalSharedMemorySizeInBytes(io_buffer_size);
  if (!entry->shared_memory.CreateAndMapAnonymous(shared_memory_size)) {
    SendErrorMessage(stream_id);
    return;
  }

  scoped_ptr<AudioSyncReader> reader(
      new AudioSyncReader(&entry->shared_memory, params, input_channels));

  if (!reader->Init()) {
    SendErrorMessage(stream_id);
    return;
  }

  entry->reader.reset(reader.release());
  entry->controller = media::AudioOutputController::Create(
      audio_manager_, this, audio_params, entry->reader.get());

  if (!entry->controller) {
    SendErrorMessage(stream_id);
    return;
  }

  entry->stream_id = stream_id;
  audio_entries_.insert(std::make_pair(stream_id, entry.release()));
  if (media_observer_)
    media_observer_->OnSetAudioStreamStatus(this, stream_id, "created");
}
