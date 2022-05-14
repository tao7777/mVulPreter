void EglRenderingVDAClient::CreateDecoder() {
   CHECK(decoder_deleted());
 #if defined(OS_WIN)
   scoped_refptr<DXVAVideoDecodeAccelerator> decoder =
      new DXVAVideoDecodeAccelerator(this);
 #else  // OS_WIN
   scoped_refptr<OmxVideoDecodeAccelerator> decoder =
       new OmxVideoDecodeAccelerator(this);
  decoder->SetEglState(egl_display(), egl_context());
#endif  // OS_WIN
  decoder_ = decoder.release();
  SetState(CS_DECODER_SET);
  if (decoder_deleted())
    return;

  media::VideoCodecProfile profile = media::H264PROFILE_BASELINE;
  if (profile_ != -1)
    profile = static_cast<media::VideoCodecProfile>(profile_);
  CHECK(decoder_->Initialize(profile));
}
