bool WebRtcAudioRenderer::Initialize(WebRtcAudioRendererSource* source) {
  base::AutoLock auto_lock(lock_);
  DCHECK_EQ(state_, UNINITIALIZED);
  DCHECK(source);
  DCHECK(!sink_);
  DCHECK(!source_);

  sink_ = AudioDeviceFactory::NewOutputDevice();
  DCHECK(sink_);

  int sample_rate = GetAudioOutputSampleRate();
  DVLOG(1) << "Audio output hardware sample rate: " << sample_rate;
  UMA_HISTOGRAM_ENUMERATION("WebRTC.AudioOutputSampleRate",
                            sample_rate, media::kUnexpectedAudioSampleRate);

  if (std::find(&kValidOutputRates[0],
                &kValidOutputRates[0] + arraysize(kValidOutputRates),
                sample_rate) ==
                    &kValidOutputRates[arraysize(kValidOutputRates)]) {
    DLOG(ERROR) << sample_rate << " is not a supported output rate.";
    return false;
  }

  media::ChannelLayout channel_layout = media::CHANNEL_LAYOUT_STEREO;

  int buffer_size = 0;

#if defined(OS_WIN)
  channel_layout = media::CHANNEL_LAYOUT_STEREO;


  if (sample_rate == 96000 || sample_rate == 48000) {
    buffer_size = (sample_rate / 100);
  } else {
    buffer_size = 2 * 440;
  }

  if (base::win::GetVersion() < base::win::VERSION_VISTA) {
    buffer_size = 3 * buffer_size;
    DLOG(WARNING) << "Extending the output buffer size by a factor of three "
                  << "since Windows XP has been detected.";
  }
#elif defined(OS_MACOSX)
  channel_layout = media::CHANNEL_LAYOUT_MONO;
 
  // frame size to use for 96kHz, 48kHz and 44.1kHz.
 
  if (sample_rate == 96000 || sample_rate == 48000) {
    buffer_size = (sample_rate / 100);
   } else {
    buffer_size = 440;
  }
#elif defined(OS_LINUX) || defined(OS_OPENBSD)
  channel_layout = media::CHANNEL_LAYOUT_MONO;

  buffer_size = 480;
#else
  DLOG(ERROR) << "Unsupported platform";
  return false;
#endif

  params_.Reset(media::AudioParameters::AUDIO_PCM_LOW_LATENCY, channel_layout,
                sample_rate, 16, buffer_size);

  buffer_.reset(new int16[params_.frames_per_buffer() * params_.channels()]);

  source_ = source;
  source->SetRenderFormat(params_);

  sink_->Initialize(params_, this);
  sink_->SetSourceRenderView(source_render_view_id_);
  sink_->Start();

  state_ = PAUSED;

  UMA_HISTOGRAM_ENUMERATION("WebRTC.AudioOutputChannelLayout",
                            channel_layout, media::CHANNEL_LAYOUT_MAX);
  UMA_HISTOGRAM_ENUMERATION("WebRTC.AudioOutputFramesPerBuffer",
                            buffer_size, kUnexpectedAudioBufferSize);
  AddHistogramFramesPerBuffer(buffer_size);

  return true;
}
