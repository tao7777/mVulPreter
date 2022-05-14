 static void set_roi_map(const vpx_codec_enc_cfg_t *cfg,
                         vpx_codec_ctx_t *codec) {
   unsigned int i;
  vpx_roi_map_t roi;
  memset(&roi, 0, sizeof(roi));
 
   roi.rows = (cfg->g_h + 15) / 16;
   roi.cols = (cfg->g_w + 15) / 16;

  roi.delta_q[0] = 0;
  roi.delta_q[1] = -2;
  roi.delta_q[2] = -4;
  roi.delta_q[3] = -6;

  roi.delta_lf[0] = 0;
  roi.delta_lf[1] = 1;
  roi.delta_lf[2] = 2;
  roi.delta_lf[3] = 3;

  roi.static_threshold[0] = 1500;
  roi.static_threshold[1] = 1000;
  roi.static_threshold[2] = 500;
  roi.static_threshold[3] = 0;

  roi.roi_map = (uint8_t *)malloc(roi.rows * roi.cols);
 for (i = 0; i < roi.rows * roi.cols; ++i)
    roi.roi_map[i] = i % 4;

 if (vpx_codec_control(codec, VP8E_SET_ROI_MAP, &roi))
    die_codec(codec, "Failed to set ROI map");

  free(roi.roi_map);
}
