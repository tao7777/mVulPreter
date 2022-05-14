 static void unset_active_map(const vpx_codec_enc_cfg_t *cfg,
                              vpx_codec_ctx_t *codec) {
  vpx_active_map_t map = {0, 0, 0};
 
   map.rows = (cfg->g_h + 15) / 16;
   map.cols = (cfg->g_w + 15) / 16;
 map.active_map = NULL;

 if (vpx_codec_control(codec, VP8E_SET_ACTIVEMAP, &map))

     die_codec(codec, "Failed to set active map");
 }
