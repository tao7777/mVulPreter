void GpuCommandBufferStub::OnCreateVideoDecoder(
    media::VideoCodecProfile profile,
    IPC::Message* reply_message) {
  int decoder_route_id = channel_->GenerateRouteID();
  GpuCommandBufferMsg_CreateVideoDecoder::WriteReplyParams(
      reply_message, decoder_route_id);
  GpuVideoDecodeAccelerator* decoder =
       new GpuVideoDecodeAccelerator(this, decoder_route_id, this);
   video_decoders_.AddWithID(decoder, decoder_route_id);
   channel_->AddRoute(decoder_route_id, decoder);
  decoder->Initialize(profile, reply_message);
 }
