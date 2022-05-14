AudioOutputDevice::AudioOutputDevice(
    AudioOutputIPC* ipc,
    const scoped_refptr<base::MessageLoopProxy>& io_loop)
    : ScopedLoopObserver(io_loop),
      input_channels_(0),
      callback_(NULL),
       ipc_(ipc),
       stream_id_(0),
       play_on_start_(true),
      is_started_(false) {
   CHECK(ipc_);
 }
