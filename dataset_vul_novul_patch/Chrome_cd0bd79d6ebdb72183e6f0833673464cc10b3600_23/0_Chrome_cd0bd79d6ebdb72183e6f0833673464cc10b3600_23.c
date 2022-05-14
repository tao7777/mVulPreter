 DXVAVideoDecodeAccelerator::DXVAVideoDecodeAccelerator(
    media::VideoDecodeAccelerator::Client* client)
     : client_(client),
       egl_config_(NULL),
       state_(kUninitialized),
       pictures_requested_(false),
       last_input_buffer_id_(-1),
       inputs_before_decode_(0) {
   memset(&input_stream_info_, 0, sizeof(input_stream_info_));
  memset(&output_stream_info_, 0, sizeof(output_stream_info_));
}
