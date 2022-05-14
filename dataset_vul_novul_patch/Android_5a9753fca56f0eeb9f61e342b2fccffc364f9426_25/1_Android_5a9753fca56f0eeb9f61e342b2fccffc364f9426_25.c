void update_rate_histogram(struct rate_hist *hist,
 const vpx_codec_enc_cfg_t *cfg,
 const vpx_codec_cx_pkt_t *pkt) {
 int i;
 int64_t then = 0;
 int64_t avg_bitrate = 0;
 int64_t sum_sz = 0;
 const int64_t now = pkt->data.frame.pts * 1000 *
 (uint64_t)cfg->g_timebase.num /
 (uint64_t)cfg->g_timebase.den;

 int idx = hist->frames++ % hist->samples;
  hist->pts[idx] = now;
  hist->sz[idx] = (int)pkt->data.frame.sz;


   if (now < cfg->rc_buf_initial_sz)
     return;
 
   then = now;
 
   /* Sum the size over the past rc_buf_sz ms */
 for (i = hist->frames; i > 0 && hist->frames - i < hist->samples; i--) {
 const int i_idx = (i - 1) % hist->samples;

    then = hist->pts[i_idx];
 if (now - then > cfg->rc_buf_sz)
 break;
    sum_sz += hist->sz[i_idx];
 }

 if (now == then)
 return;

  avg_bitrate = sum_sz * 8 * 1000 / (now - then);
  idx = (int)(avg_bitrate * (RATE_BINS / 2) / (cfg->rc_target_bitrate * 1000));
 if (idx < 0)
    idx = 0;
 if (idx > RATE_BINS - 1)
    idx = RATE_BINS - 1;
 if (hist->bucket[idx].low > avg_bitrate)
    hist->bucket[idx].low = (int)avg_bitrate;
 if (hist->bucket[idx].high < avg_bitrate)
    hist->bucket[idx].high = (int)avg_bitrate;
  hist->bucket[idx].count++;
  hist->total++;
}
