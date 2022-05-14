int vp78_decode_frame(AVCodecContext *avctx, void *data, int *got_frame,
                      AVPacket *avpkt, int is_vp7)
{
    VP8Context *s = avctx->priv_data;
    int ret, i, referenced, num_jobs;
     enum AVDiscard skip_thresh;
     VP8Frame *av_uninit(curframe), *prev_frame;
 
    av_assert0(avctx->pix_fmt == AV_PIX_FMT_YUVA420P || avctx->pix_fmt == AV_PIX_FMT_YUV420P);

     if (is_vp7)
         ret = vp7_decode_frame_header(s, avpkt->data, avpkt->size);
     else
        ret = vp8_decode_frame_header(s, avpkt->data, avpkt->size);

    if (ret < 0)
        goto err;

    prev_frame = s->framep[VP56_FRAME_CURRENT];

    referenced = s->update_last || s->update_golden == VP56_FRAME_CURRENT ||
                 s->update_altref == VP56_FRAME_CURRENT;

    skip_thresh = !referenced ? AVDISCARD_NONREF
                              : !s->keyframe ? AVDISCARD_NONKEY
                                             : AVDISCARD_ALL;

    if (avctx->skip_frame >= skip_thresh) {
        s->invisible = 1;
        memcpy(&s->next_framep[0], &s->framep[0], sizeof(s->framep[0]) * 4);
        goto skip_decode;
    }
    s->deblock_filter = s->filter.level && avctx->skip_loop_filter < skip_thresh;

    for (i = 0; i < 5; i++)
        if (s->frames[i].tf.f->data[0] &&
            &s->frames[i] != prev_frame &&
            &s->frames[i] != s->framep[VP56_FRAME_PREVIOUS] &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN]   &&
            &s->frames[i] != s->framep[VP56_FRAME_GOLDEN2])
            vp8_release_frame(s, &s->frames[i]);

    curframe = s->framep[VP56_FRAME_CURRENT] = vp8_find_free_buffer(s);

    if (!s->colorspace)
        avctx->colorspace = AVCOL_SPC_BT470BG;
    if (s->fullrange)
        avctx->color_range = AVCOL_RANGE_JPEG;
    else
        avctx->color_range = AVCOL_RANGE_MPEG;

    /* Given that arithmetic probabilities are updated every frame, it's quite
     * likely that the values we have on a random interframe are complete
     * junk if we didn't start decode on a keyframe. So just don't display
     * anything rather than junk. */
    if (!s->keyframe && (!s->framep[VP56_FRAME_PREVIOUS] ||
                         !s->framep[VP56_FRAME_GOLDEN]   ||
                         !s->framep[VP56_FRAME_GOLDEN2])) {
        av_log(avctx, AV_LOG_WARNING,
               "Discarding interframe without a prior keyframe!\n");
        ret = AVERROR_INVALIDDATA;
        goto err;
    }

    curframe->tf.f->key_frame = s->keyframe;
    curframe->tf.f->pict_type = s->keyframe ? AV_PICTURE_TYPE_I
                                            : AV_PICTURE_TYPE_P;
    if ((ret = vp8_alloc_frame(s, curframe, referenced)) < 0)
        goto err;

    if (s->update_altref != VP56_FRAME_NONE)
        s->next_framep[VP56_FRAME_GOLDEN2] = s->framep[s->update_altref];
    else
        s->next_framep[VP56_FRAME_GOLDEN2] = s->framep[VP56_FRAME_GOLDEN2];

    if (s->update_golden != VP56_FRAME_NONE)
        s->next_framep[VP56_FRAME_GOLDEN] = s->framep[s->update_golden];
    else
        s->next_framep[VP56_FRAME_GOLDEN] = s->framep[VP56_FRAME_GOLDEN];

    if (s->update_last)
        s->next_framep[VP56_FRAME_PREVIOUS] = curframe;
    else
        s->next_framep[VP56_FRAME_PREVIOUS] = s->framep[VP56_FRAME_PREVIOUS];

    s->next_framep[VP56_FRAME_CURRENT] = curframe;

    if (avctx->codec->update_thread_context)
        ff_thread_finish_setup(avctx);

    s->linesize   = curframe->tf.f->linesize[0];
    s->uvlinesize = curframe->tf.f->linesize[1];

    memset(s->top_nnz, 0, s->mb_width * sizeof(*s->top_nnz));
    /* Zero macroblock structures for top/top-left prediction
     * from outside the frame. */
    if (!s->mb_layout)
        memset(s->macroblocks + s->mb_height * 2 - 1, 0,
               (s->mb_width + 1) * sizeof(*s->macroblocks));
    if (!s->mb_layout && s->keyframe)
        memset(s->intra4x4_pred_mode_top, DC_PRED, s->mb_width * 4);

    memset(s->ref_count, 0, sizeof(s->ref_count));

    if (s->mb_layout == 1) {
        if (prev_frame && s->segmentation.enabled &&
            !s->segmentation.update_map)
            ff_thread_await_progress(&prev_frame->tf, 1, 0);
        if (is_vp7)
            vp7_decode_mv_mb_modes(avctx, curframe, prev_frame);
        else
            vp8_decode_mv_mb_modes(avctx, curframe, prev_frame);
    }

    if (avctx->active_thread_type == FF_THREAD_FRAME)
        num_jobs = 1;
    else
        num_jobs = FFMIN(s->num_coeff_partitions, avctx->thread_count);
    s->num_jobs   = num_jobs;
    s->curframe   = curframe;
    s->prev_frame = prev_frame;
    s->mv_bounds.mv_min.y   = -MARGIN;
    s->mv_bounds.mv_max.y   = ((s->mb_height - 1) << 6) + MARGIN;
    for (i = 0; i < MAX_THREADS; i++) {
        VP8ThreadData *td = &s->thread_data[i];
        atomic_init(&td->thread_mb_pos, 0);
        atomic_init(&td->wait_mb_pos, INT_MAX);
    }
    if (is_vp7)
        avctx->execute2(avctx, vp7_decode_mb_row_sliced, s->thread_data, NULL,
                        num_jobs);
    else
        avctx->execute2(avctx, vp8_decode_mb_row_sliced, s->thread_data, NULL,
                        num_jobs);

    ff_thread_report_progress(&curframe->tf, INT_MAX, 0);
    memcpy(&s->framep[0], &s->next_framep[0], sizeof(s->framep[0]) * 4);

skip_decode:
    if (!s->update_probabilities)
        s->prob[0] = s->prob[1];

    if (!s->invisible) {
        if ((ret = av_frame_ref(data, curframe->tf.f)) < 0)
            return ret;
        *got_frame = 1;
    }

    return avpkt->size;
err:
    memcpy(&s->next_framep[0], &s->framep[0], sizeof(s->framep[0]) * 4);
    return ret;
}
