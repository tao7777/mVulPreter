void DXVAVideoDecodeAccelerator::Decode(
    const media::BitstreamBuffer& bitstream_buffer) {
  DCHECK(CalledOnValidThread());

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kNormal || state_ == kStopped),
      "Invalid state: " << state_, ILLEGAL_STATE,);
 
   base::win::ScopedComPtr<IMFSample> sample;
   sample.Attach(CreateSampleFromInputBuffer(bitstream_buffer,
                                            renderer_process_,
                                             input_stream_info_.cbSize,
                                             input_stream_info_.cbAlignment));
   RETURN_AND_NOTIFY_ON_FAILURE(sample, "Failed to create input sample",
                               PLATFORM_FAILURE,);
  if (!inputs_before_decode_) {
    TRACE_EVENT_BEGIN_ETW("DXVAVideoDecodeAccelerator.Decoding", this, "");
  }
  inputs_before_decode_++;

  RETURN_AND_NOTIFY_ON_FAILURE(
      SendMFTMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0),
      "Failed to create input sample", PLATFORM_FAILURE,);

  HRESULT hr = decoder_->ProcessInput(0, sample, 0);
  RETURN_AND_NOTIFY_ON_HR_FAILURE(hr, "Failed to process input sample",
      PLATFORM_FAILURE,);

  RETURN_AND_NOTIFY_ON_FAILURE(
    SendMFTMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0),
    "Failed to send eos message to MFT", PLATFORM_FAILURE,);
  state_ = kEosDrain;

  last_input_buffer_id_ = bitstream_buffer.id();

  DoDecode();

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kStopped || state_ == kNormal),
      "Failed to process output. Unexpected decoder state: " << state_,
      ILLEGAL_STATE,);

  MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &DXVAVideoDecodeAccelerator::NotifyInputBufferRead, this,
      bitstream_buffer.id()));
}
