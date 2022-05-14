pdf14_pop_transparency_group(gs_gstate *pgs, pdf14_ctx *ctx,
    const pdf14_nonseparable_blending_procs_t * pblend_procs,
    int tos_num_color_comp, cmm_profile_t *curr_icc_profile, gx_device *dev)
{
    pdf14_buf *tos = ctx->stack;
    pdf14_buf *nos = tos->saved;
    pdf14_mask_t *mask_stack = tos->mask_stack;
    pdf14_buf *maskbuf;
    int x0, x1, y0, y1;
    byte *new_data_buf = NULL;
    int num_noncolor_planes, new_num_planes;
    int num_cols, num_rows, nos_num_color_comp;
    bool icc_match;
    gsicc_rendering_param_t rendering_params;
    gsicc_link_t *icc_link;
    gsicc_bufferdesc_t input_buff_desc;
    gsicc_bufferdesc_t output_buff_desc;
    pdf14_device *pdev = (pdf14_device *)dev;
    bool overprint = pdev->overprint;
     gx_color_index drawn_comps = pdev->drawn_comps;
     bool nonicc_conversion = true;
 
    if (nos == NULL)
        return_error(gs_error_unknownerror);  /* Unmatched group pop */

     nos_num_color_comp = nos->parent_color_info_procs->num_components - nos->num_spots;
     tos_num_color_comp = tos_num_color_comp - tos->num_spots;
    if (mask_stack == NULL) {
        maskbuf = NULL;
    } else {
        maskbuf = mask_stack->rc_mask->mask_buf;
    }
    if (nos == NULL)
        return_error(gs_error_rangecheck);
    /* Sanitise the dirty rectangles, in case some of the drawing routines
     * have made them overly large. */
    rect_intersect(tos->dirty, tos->rect);
    rect_intersect(nos->dirty, nos->rect);
    /* dirty = the marked bbox. rect = the entire bounds of the buffer. */
    /* Everything marked on tos that fits onto nos needs to be merged down. */
    y0 = max(tos->dirty.p.y, nos->rect.p.y);
    y1 = min(tos->dirty.q.y, nos->rect.q.y);
    x0 = max(tos->dirty.p.x, nos->rect.p.x);
    x1 = min(tos->dirty.q.x, nos->rect.q.x);
    if (ctx->mask_stack) {
        /* This can occur when we have a situation where we are ending out of
           a group that has internal to it a soft mask and another group.
           The soft mask left over from the previous trans group pop is put
           into ctx->masbuf, since it is still active if another trans group
           push occurs to use it.  If one does not occur, but instead we find
           ourselves popping from a parent group, then this softmask is no
           longer needed.  We will rc_decrement and set it to NULL. */
        rc_decrement(ctx->mask_stack->rc_mask, "pdf14_pop_transparency_group");
        if (ctx->mask_stack->rc_mask == NULL ){
            gs_free_object(ctx->memory, ctx->mask_stack, "pdf14_pop_transparency_group");
        }
        ctx->mask_stack = NULL;
    }
    ctx->mask_stack = mask_stack;  /* Restore the mask saved by pdf14_push_transparency_group. */
    tos->mask_stack = NULL;        /* Clean the pointer sinse the mask ownership is now passed to ctx. */
    if (tos->idle)
        goto exit;
    if (maskbuf != NULL && maskbuf->data == NULL && maskbuf->alpha == 255)
        goto exit;

#if RAW_DUMP
    /* Dump the current buffer to see what we have. */
    dump_raw_buffer(ctx->stack->rect.q.y-ctx->stack->rect.p.y,
                ctx->stack->rowstride, ctx->stack->n_planes,
                ctx->stack->planestride, ctx->stack->rowstride,
                "aaTrans_Group_Pop",ctx->stack->data);
#endif
/* Note currently if a pattern space has transparency, the ICC profile is not used
   for blending purposes.  Instead we rely upon the gray, rgb, or cmyk parent space.
   This is partially due to the fact that pdf14_pop_transparency_group and
   pdf14_push_transparnecy_group have no real ICC interaction and those are the
   operations called in the tile transparency code.  Instead we may want to
   look at pdf14_begin_transparency_group and pdf14_end_transparency group which
   is where all the ICC information is handled.  We will return to look at that later */
    if (nos->parent_color_info_procs->icc_profile != NULL) {
        icc_match = (nos->parent_color_info_procs->icc_profile->hashcode !=
                        curr_icc_profile->hashcode);
    } else {
        /* Let the other tests make the decision if we need to transform */
        icc_match = false;
    }
    /* If the color spaces are different and we actually did do a swap of
       the procs for color */
    if ((nos->parent_color_info_procs->parent_color_mapping_procs != NULL &&
        nos_num_color_comp != tos_num_color_comp) || icc_match) {
        if (x0 < x1 && y0 < y1) {
            /* The NOS blending color space is different than that of the
               TOS.  It is necessary to transform the TOS buffer data to the
               color space of the NOS prior to doing the pdf14_compose_group
               operation.  */
            num_noncolor_planes = tos->n_planes - tos_num_color_comp;
            new_num_planes = num_noncolor_planes + nos_num_color_comp;

            /* See if we are doing ICC based conversion */
            if (nos->parent_color_info_procs->icc_profile != NULL &&
                curr_icc_profile != NULL) {
                /* Use the ICC color management for buffer color conversion */
                /* Define the rendering intents */
                rendering_params.black_point_comp = gsBLACKPTCOMP_ON;
                rendering_params.graphics_type_tag = GS_IMAGE_TAG;
                rendering_params.override_icc = false;
                rendering_params.preserve_black = gsBKPRESNOTSPECIFIED;
                rendering_params.rendering_intent = gsPERCEPTUAL;
                rendering_params.cmm = gsCMM_DEFAULT;
                /* Request the ICC link for the transform that we will need to use */
                /* Note that if pgs is NULL we assume the same color space.  This
                   is due to a call to pop the group from fill_mask when filling
                   with a mask with transparency.  In that case, the parent
                   and the child will have the same color space anyway */
                icc_link = gsicc_get_link_profile(pgs, dev, curr_icc_profile,
                                    nos->parent_color_info_procs->icc_profile,
                                    &rendering_params, pgs->memory, false);
                if (icc_link != NULL) {
                    /* if problem with link we will do non-ICC approach */
                    nonicc_conversion = false;
                    /* If the link is the identity, then we don't need to do
                       any color conversions */
                    if ( !(icc_link->is_identity) ) {
                        /* Before we do any allocations check if we can get away with
                           reusing the existing buffer if it is the same size ( if it is
                           smaller go ahead and allocate).  We could reuse it in this
                           case too.  We need to do a bit of testing to determine what
                           would be best.  */
                        /* FIXME: RJW: Could we get away with just color converting
                         * the area that's actually active (i.e. dirty, not rect)?
                         */
                        if(nos_num_color_comp != tos_num_color_comp) {
                            /* Different size.  We will need to allocate */
                            new_data_buf = gs_alloc_bytes(ctx->memory,
                                                tos->planestride * new_num_planes,
                                                    "pdf14_pop_transparency_group");
                            if (new_data_buf == NULL)
                                return_error(gs_error_VMerror);
                            /* Copy over the noncolor planes. */
                            memcpy(new_data_buf + tos->planestride * nos_num_color_comp,
                                   tos->data + tos->planestride * tos_num_color_comp,
                                   tos->planestride * num_noncolor_planes);
                        } else {
                            /* In place color conversion! */
                            new_data_buf = tos->data;
                        }
                        /* Set up the buffer descriptors. Note that pdf14 always has
                           the alpha channels at the back end (last planes).
                           We will just handle that here and let the CMM know
                           nothing about it */
                        num_rows = tos->rect.q.y - tos->rect.p.y;
                        num_cols = tos->rect.q.x - tos->rect.p.x;
                        gsicc_init_buffer(&input_buff_desc, tos_num_color_comp, 1,
                                          false, false, true,
                                          tos->planestride, tos->rowstride,
                                          num_rows, num_cols);
                        gsicc_init_buffer(&output_buff_desc, nos_num_color_comp,
                                          1, false, false, true, tos->planestride,
                                          tos->rowstride, num_rows, num_cols);
                        /* Transform the data. Since the pdf14 device should be
                           using RGB, CMYK or Gray buffers, this transform
                           does not need to worry about the cmap procs of
                           the target device.  Those are handled when we do
                           the pdf14 put image operation */
                        (icc_link->procs.map_buffer)(dev, icc_link, &input_buff_desc,
                                                     &output_buff_desc, tos->data,
                                                     new_data_buf);
                    }
                    /* Release the link */
                    gsicc_release_link(icc_link);
                    /* free the old object if the color spaces were different sizes */
                    if(!(icc_link->is_identity) &&
                        nos_num_color_comp != tos_num_color_comp) {
                        gs_free_object(ctx->memory, tos->data,
                            "pdf14_pop_transparency_group");
                        tos->data = new_data_buf;
                    }
                }
            }
            if (nonicc_conversion) {
                /* Non ICC based transform */
                new_data_buf = gs_alloc_bytes(ctx->memory,
                                    tos->planestride * new_num_planes,
                                    "pdf14_pop_transparency_group");
                if (new_data_buf == NULL)
                    return_error(gs_error_VMerror);
                gs_transform_color_buffer_generic(tos->data, tos->rowstride,
                            tos->planestride, tos_num_color_comp, tos->rect,
                            new_data_buf, nos_num_color_comp, num_noncolor_planes);
                /* Free the old object */
                gs_free_object(ctx->memory, tos->data,
                                "pdf14_pop_transparency_group");
                 tos->data = new_data_buf;
            }
             /* Adjust the plane and channel size now */
             tos->n_chan = nos->n_chan;
             tos->n_planes = nos->n_planes;
#if RAW_DUMP
            /* Dump the current buffer to see what we have. */
            dump_raw_buffer(ctx->stack->rect.q.y-ctx->stack->rect.p.y,
                            ctx->stack->rowstride, ctx->stack->n_chan,
                            ctx->stack->planestride, ctx->stack->rowstride,
                            "aCMTrans_Group_ColorConv",ctx->stack->data);
#endif
             /* compose. never do overprint in this case */
            pdf14_compose_group(tos, nos, maskbuf, x0, x1, y0, y1, nos->n_chan,
                 nos->parent_color_info_procs->isadditive,
                 nos->parent_color_info_procs->parent_blending_procs,
                 false, drawn_comps, ctx->memory, dev);
        }
    } else {
        /* Group color spaces are the same.  No color conversions needed */
        if (x0 < x1 && y0 < y1)
            pdf14_compose_group(tos, nos, maskbuf, x0, x1, y0, y1, nos->n_chan,
                                ctx->additive, pblend_procs, overprint,
                                drawn_comps, ctx->memory, dev);
    }
exit:
    ctx->stack = nos;
    /* We want to detect the cases where we have luminosity soft masks embedded
       within one another.  The "alpha" channel really needs to be merged into
       the luminosity channel in this case.  This will occur during the mask pop */
    if (ctx->smask_depth > 0 && maskbuf != NULL) {
        /* Set the trigger so that we will blend if not alpha. Since
           we have softmasks embedded in softmasks */
        ctx->smask_blend = true;
    }
    if_debug1m('v', ctx->memory, "[v]pop buf, idle=%d\n", tos->idle);
    pdf14_buf_free(tos, ctx->memory);
    return 0;
}
