 TileIndependenceTest()
 : EncoderTest(GET_PARAM(0)),
        md5_fw_order_(),

         md5_inv_order_(),
         n_tiles_(GET_PARAM(1)) {
     init_flags_ = VPX_CODEC_USE_PSNR;
    vpx_codec_dec_cfg_t cfg;
     cfg.w = 704;
     cfg.h = 144;
     cfg.threads = 1;
    fw_dec_ = codec_->CreateDecoder(cfg, 0);
    inv_dec_ = codec_->CreateDecoder(cfg, 0);
    inv_dec_->Control(VP9_INVERT_TILE_DECODE_ORDER, 1);
 }
