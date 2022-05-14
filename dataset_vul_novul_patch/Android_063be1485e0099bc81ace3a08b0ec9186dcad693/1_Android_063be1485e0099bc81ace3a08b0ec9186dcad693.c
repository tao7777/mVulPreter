static vpx_codec_err_t vp8_decode(vpx_codec_alg_priv_t *ctx,
 const uint8_t *data,
 unsigned int            data_sz,
 void *user_priv,
 long                    deadline)
{
 vpx_codec_err_t res = VPX_CODEC_OK;
 unsigned int resolution_change = 0;
 unsigned int w, h;

 if (!ctx->fragments.enabled && (data == NULL && data_sz == 0))
 {
 return 0;
 }

 /* Update the input fragment data */
 if(update_fragments(ctx, data, data_sz, &res) <= 0)
 return res;

 /* Determine the stream parameters. Note that we rely on peek_si to
     * validate that we have a buffer that does not wrap around the top
     * of the heap.
     */
    w = ctx->si.w;
    h = ctx->si.h;

    res = vp8_peek_si_internal(ctx->fragments.ptrs[0], ctx->fragments.sizes[0],
 &ctx->si, ctx->decrypt_cb, ctx->decrypt_state);

 if((res == VPX_CODEC_UNSUP_BITSTREAM) && !ctx->si.is_kf)
 {
 /* the peek function returns an error for non keyframes, however for
         * this case, it is not an error */
        res = VPX_CODEC_OK;
 }

 if(!ctx->decoder_init && !ctx->si.is_kf)
        res = VPX_CODEC_UNSUP_BITSTREAM;

 if ((ctx->si.h != h) || (ctx->si.w != w))
        resolution_change = 1;

 /* Initialize the decoder instance on the first frame*/
 if (!res && !ctx->decoder_init)
 {
      VP8D_CONFIG oxcf;

      oxcf.Width = ctx->si.w;
      oxcf.Height = ctx->si.h;
      oxcf.Version = 9;
      oxcf.postprocess = 0;
      oxcf.max_threads = ctx->cfg.threads;
      oxcf.error_concealment =
 (ctx->base.init_flags & VPX_CODEC_USE_ERROR_CONCEALMENT);

 /* If postprocessing was enabled by the application and a
       * configuration has not been provided, default it.
       */
 if (!ctx->postproc_cfg_set
 && (ctx->base.init_flags & VPX_CODEC_USE_POSTPROC)) {
         ctx->postproc_cfg.post_proc_flag =
             VP8_DEBLOCK | VP8_DEMACROBLOCK | VP8_MFQE;
         ctx->postproc_cfg.deblocking_level = 4;
         ctx->postproc_cfg.noise_level = 0;
 }

       res = vp8_create_decoder_instances(&ctx->yv12_frame_buffers, &oxcf);
       ctx->decoder_init = 1;
 }

 /* Set these even if already initialized.  The caller may have changed the
     * decrypt config between frames.
     */
 if (ctx->decoder_init) {
      ctx->yv12_frame_buffers.pbi[0]->decrypt_cb = ctx->decrypt_cb;
      ctx->yv12_frame_buffers.pbi[0]->decrypt_state = ctx->decrypt_state;
 }

 if (!res)
 {
        VP8D_COMP *pbi = ctx->yv12_frame_buffers.pbi[0];
 if (resolution_change)
 {
            VP8_COMMON *const pc = & pbi->common;
            MACROBLOCKD *const xd  = & pbi->mb;
#if CONFIG_MULTITHREAD
 int i;
#endif
            pc->Width = ctx->si.w;
            pc->Height = ctx->si.h;
 {
 int prev_mb_rows = pc->mb_rows;


                 if (setjmp(pbi->common.error.jmp))
                 {
                     pbi->common.error.setjmp = 0;
                     vp8_clear_system_state();
                     /* same return value as used in vp8dx_receive_compressed_data */
                     return -1;
 }

                pbi->common.error.setjmp = 1;

 if (pc->Width <= 0)
 {
                    pc->Width = w;
                    vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
 "Invalid frame width");
 }

 if (pc->Height <= 0)
 {
                    pc->Height = h;
                    vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
 "Invalid frame height");
 }

 if (vp8_alloc_frame_buffers(pc, pc->Width, pc->Height))
                    vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
 "Failed to allocate frame buffers");

                xd->pre = pc->yv12_fb[pc->lst_fb_idx];
                xd->dst = pc->yv12_fb[pc->new_fb_idx];

#if CONFIG_MULTITHREAD
 for (i = 0; i < pbi->allocated_decoding_thread_count; i++)
 {
                    pbi->mb_row_di[i].mbd.dst = pc->yv12_fb[pc->new_fb_idx];
                    vp8_build_block_doffsets(&pbi->mb_row_di[i].mbd);
 }
#endif
                vp8_build_block_doffsets(&pbi->mb);

 /* allocate memory for last frame MODE_INFO array */
#if CONFIG_ERROR_CONCEALMENT

 if (pbi->ec_enabled)
 {
 /* old prev_mip was released by vp8_de_alloc_frame_buffers()
                     * called in vp8_alloc_frame_buffers() */
                    pc->prev_mip = vpx_calloc(
 (pc->mb_cols + 1) * (pc->mb_rows + 1),
 sizeof(MODE_INFO));

 if (!pc->prev_mip)
 {
                        vp8_de_alloc_frame_buffers(pc);
                        vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
 "Failed to allocate"
 "last frame MODE_INFO array");
 }

                    pc->prev_mi = pc->prev_mip + pc->mode_info_stride + 1;

 if (vp8_alloc_overlap_lists(pbi))
                        vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
 "Failed to allocate overlap lists "
 "for error concealment");
 }

#endif

#if CONFIG_MULTITHREAD
 if (pbi->b_multithreaded_rd)
                    vp8mt_alloc_temp_buffers(pbi, pc->Width, prev_mb_rows);
#else
 (void)prev_mb_rows;
#endif
 }

            pbi->common.error.setjmp = 0;

 /* required to get past the first get_free_fb() call */
            pbi->common.fb_idx_ref_cnt[0] = 0;
 }

 /* update the pbi fragment data */
        pbi->fragments = ctx->fragments;

        ctx->user_priv = user_priv;
 if (vp8dx_receive_compressed_data(pbi, data_sz, data, deadline))
 {
            res = update_error_state(ctx, &pbi->common.error);
 }

 /* get ready for the next series of fragments */
        ctx->fragments.count = 0;
 }

 return res;
}
