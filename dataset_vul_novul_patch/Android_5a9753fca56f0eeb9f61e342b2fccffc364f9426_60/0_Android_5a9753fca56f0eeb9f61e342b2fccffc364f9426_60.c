void DecoderTest::RunLoop(CompressedVideoSource *video) {
bool Decoder::IsVP8() const {
  const char *codec_name = GetDecoderName();
  return strncmp(kVP8Name, codec_name, sizeof(kVP8Name) - 1) == 0;
}

void DecoderTest::HandlePeekResult(Decoder *const decoder,
                                   CompressedVideoSource *video,
                                   const vpx_codec_err_t res_peek) {
  const bool is_vp8 = decoder->IsVP8();
  if (is_vp8) {
    /* Vp8's implementation of PeekStream returns an error if the frame you
     * pass it is not a keyframe, so we only expect VPX_CODEC_OK on the first
     * frame, which must be a keyframe. */
    if (video->frame_number() == 0)
      ASSERT_EQ(VPX_CODEC_OK, res_peek) << "Peek return failed: "
                                        << vpx_codec_err_to_string(res_peek);
  } else {
    /* The Vp9 implementation of PeekStream returns an error only if the
     * data passed to it isn't a valid Vp9 chunk. */
    ASSERT_EQ(VPX_CODEC_OK, res_peek) << "Peek return failed: "
                                      << vpx_codec_err_to_string(res_peek);
  }
}

void DecoderTest::RunLoop(CompressedVideoSource *video,
                          const vpx_codec_dec_cfg_t &dec_cfg) {
  Decoder* const decoder = codec_->CreateDecoder(dec_cfg, flags_, 0);
   ASSERT_TRUE(decoder != NULL);
  bool end_of_file = false;
 
  for (video->Begin(); !::testing::Test::HasFailure() && !end_of_file;
       video->Next()) {
     PreDecodeFrameHook(*video, decoder);

    vpx_codec_stream_info_t stream_info;
    stream_info.sz = sizeof(stream_info);

    if (video->cxdata() != NULL) {
      const vpx_codec_err_t res_peek = decoder->PeekStream(video->cxdata(),
                                                           video->frame_size(),
                                                           &stream_info);
      HandlePeekResult(decoder, video, res_peek);
      ASSERT_FALSE(::testing::Test::HasFailure());

      vpx_codec_err_t res_dec = decoder->DecodeFrame(video->cxdata(),
                                                     video->frame_size());
      if (!HandleDecodeResult(res_dec, *video, decoder))
        break;
    } else {
      // Signal end of the file to the decoder.
      const vpx_codec_err_t res_dec = decoder->DecodeFrame(NULL, 0);
      ASSERT_EQ(VPX_CODEC_OK, res_dec) << decoder->DecodeError();
      end_of_file = true;
    }
 
     DxDataIterator dec_iter = decoder->GetDxData();
     const vpx_image_t *img = NULL;


     while ((img = dec_iter.Next()))
       DecompressedFrameHook(*img, video->frame_number());
   }
   delete decoder;
 }
