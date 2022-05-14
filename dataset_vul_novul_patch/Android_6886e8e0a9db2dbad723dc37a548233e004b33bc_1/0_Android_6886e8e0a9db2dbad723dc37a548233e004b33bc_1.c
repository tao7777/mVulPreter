int vp8dx_receive_compressed_data(VP8D_COMP *pbi, size_t size,
 const uint8_t *source,
 int64_t time_stamp)
{
    VP8_COMMON *cm = &pbi->common;
 int retcode = -1;
 (void)size;
 (void)source;

    pbi->common.error.error_code = VPX_CODEC_OK;

    retcode = check_fragments_for_errors(pbi);
 if(retcode <= 0)
 return retcode;

    cm->new_fb_idx = get_free_fb (cm);

 /* setup reference frames for vp8_decode_frame */
    pbi->dec_fb_ref[INTRA_FRAME] = &cm->yv12_fb[cm->new_fb_idx];
    pbi->dec_fb_ref[LAST_FRAME] = &cm->yv12_fb[cm->lst_fb_idx];
    pbi->dec_fb_ref[GOLDEN_FRAME] = &cm->yv12_fb[cm->gld_fb_idx];
    pbi->dec_fb_ref[ALTREF_FRAME] = &cm->yv12_fb[cm->alt_fb_idx];

 if (setjmp(pbi->common.error.jmp))
 {
 /* We do not know if the missing frame(s) was supposed to update
        * any of the reference buffers, but we act conservative and
        * mark only the last buffer as corrupted.
        */
        cm->yv12_fb[cm->lst_fb_idx].corrupted = 1;

 
         if (cm->fb_idx_ref_cnt[cm->new_fb_idx] > 0)
           cm->fb_idx_ref_cnt[cm->new_fb_idx]--;
         goto decode_exit;
     }
 
    pbi->common.error.setjmp = 1;

    retcode = vp8_decode_frame(pbi);

 if (retcode < 0)
 {
 if (cm->fb_idx_ref_cnt[cm->new_fb_idx] > 0)
          cm->fb_idx_ref_cnt[cm->new_fb_idx]--;

        pbi->common.error.error_code = VPX_CODEC_ERROR;
 goto decode_exit;
 }

 if (swap_frame_buffers (cm))
 {
        pbi->common.error.error_code = VPX_CODEC_ERROR;
 goto decode_exit;
 }

    vp8_clear_system_state();

 if (cm->show_frame)
 {
        cm->current_video_frame++;
        cm->show_frame_mi = cm->mi;
 }

 #if CONFIG_ERROR_CONCEALMENT
 /* swap the mode infos to storage for future error concealment */
 if (pbi->ec_enabled && pbi->common.prev_mi)
 {
        MODE_INFO* tmp = pbi->common.prev_mi;
 int row, col;
        pbi->common.prev_mi = pbi->common.mi;
        pbi->common.mi = tmp;

 /* Propagate the segment_ids to the next frame */
 for (row = 0; row < pbi->common.mb_rows; ++row)
 {
 for (col = 0; col < pbi->common.mb_cols; ++col)
 {
 const int i = row*pbi->common.mode_info_stride + col;
                pbi->common.mi[i].mbmi.segment_id =
                        pbi->common.prev_mi[i].mbmi.segment_id;
 }
 }
 }
#endif

    pbi->ready_for_new_data = 0;
    pbi->last_time_stamp = time_stamp;

decode_exit:
    pbi->common.error.setjmp = 0;
    vp8_clear_system_state();
 return retcode;
}
