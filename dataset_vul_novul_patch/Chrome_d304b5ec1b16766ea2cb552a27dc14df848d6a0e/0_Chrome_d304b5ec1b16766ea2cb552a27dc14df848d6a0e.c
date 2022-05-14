void FFmpegVideoDecodeEngine::Initialize(
    MessageLoop* message_loop,
    VideoDecodeEngine::EventHandler* event_handler,
    VideoDecodeContext* context,
    const VideoDecoderConfig& config) {
  static const int kDecodeThreads = 2;
  static const int kMaxDecodeThreads = 16;

  codec_context_ = avcodec_alloc_context();

  codec_context_->pix_fmt = PIX_FMT_YUV420P;
  codec_context_->codec_type = AVMEDIA_TYPE_VIDEO;
  codec_context_->codec_id = VideoCodecToCodecID(config.codec());
  codec_context_->coded_width = config.width();
  codec_context_->coded_height = config.height();

  frame_rate_numerator_ = config.frame_rate_numerator();
  frame_rate_denominator_ = config.frame_rate_denominator();
 
   if (config.extra_data() != NULL) {
     codec_context_->extradata_size = config.extra_data_size();
    codec_context_->extradata = reinterpret_cast<uint8_t*>(
        av_malloc(config.extra_data_size() + FF_INPUT_BUFFER_PADDING_SIZE));
     memcpy(codec_context_->extradata, config.extra_data(),
            config.extra_data_size());
    memset(codec_context_->extradata + config.extra_data_size(), '\0',
           FF_INPUT_BUFFER_PADDING_SIZE);
   }
 
  codec_context_->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
  codec_context_->error_recognition = FF_ER_CAREFUL;

  AVCodec* codec = avcodec_find_decoder(codec_context_->codec_id);

  int decode_threads = (codec_context_->codec_id == CODEC_ID_THEORA) ?
      1 : kDecodeThreads;

  const CommandLine* cmd_line = CommandLine::ForCurrentProcess();
  std::string threads(cmd_line->GetSwitchValueASCII(switches::kVideoThreads));
  if ((!threads.empty() &&
      !base::StringToInt(threads, &decode_threads)) ||
      decode_threads < 0 || decode_threads > kMaxDecodeThreads) {
    decode_threads = kDecodeThreads;
  }

  av_frame_.reset(avcodec_alloc_frame());
  VideoCodecInfo info;
  info.success = false;
  info.provides_buffers = true;
  info.stream_info.surface_type = VideoFrame::TYPE_SYSTEM_MEMORY;
  info.stream_info.surface_format = GetSurfaceFormat();
  info.stream_info.surface_width = config.surface_width();
  info.stream_info.surface_height = config.surface_height();

  bool buffer_allocated = true;
  frame_queue_available_.clear();

  for (size_t i = 0; i < Limits::kMaxVideoFrames; ++i) {
    scoped_refptr<VideoFrame> video_frame;
    VideoFrame::CreateFrame(VideoFrame::YV12,
                            config.width(),
                            config.height(),
                            kNoTimestamp,
                            kNoTimestamp,
                            &video_frame);
    if (!video_frame.get()) {
      buffer_allocated = false;
      break;
    }
    frame_queue_available_.push_back(video_frame);
  }

  if (codec &&
      avcodec_thread_init(codec_context_, decode_threads) >= 0 &&
      avcodec_open(codec_context_, codec) >= 0 &&
      av_frame_.get() &&
      buffer_allocated) {
    info.success = true;
  }
  event_handler_ = event_handler;
  event_handler_->OnInitializeComplete(info);
}
