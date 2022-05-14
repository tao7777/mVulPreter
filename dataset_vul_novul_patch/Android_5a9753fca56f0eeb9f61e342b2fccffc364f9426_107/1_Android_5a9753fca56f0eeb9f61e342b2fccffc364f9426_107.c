   virtual void TearDown() {
    vpx_svc_release(&svc_);
     delete(decoder_);
     if (codec_initialized_) vpx_codec_destroy(&codec_);
   }
