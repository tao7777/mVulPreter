   FramebufferInfoTest()
      : manager_(1, 1),
         feature_info_(new FeatureInfo()),
         renderbuffer_manager_(NULL, kMaxRenderbufferSize, kMaxSamples,
                               kDepth24Supported) {
    texture_manager_.reset(new TextureManager(NULL,
                                              feature_info_.get(),
                                              kMaxTextureSize,
                                              kMaxCubemapSize,
                                              kUseDefaultTextures));
  }
