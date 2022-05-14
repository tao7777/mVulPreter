IHEVCD_ERROR_T ihevcd_parse_slice_data(codec_t *ps_codec)
{

    IHEVCD_ERROR_T ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
    WORD32 end_of_slice_flag = 0;
 sps_t *ps_sps;
 pps_t *ps_pps;
 slice_header_t *ps_slice_hdr;
    WORD32 end_of_pic;
 tile_t *ps_tile, *ps_tile_prev;
    WORD32 i;
    WORD32 ctb_addr;
    WORD32 tile_idx;
    WORD32 cabac_init_idc;
    WORD32 ctb_size;
    WORD32 num_ctb_in_row;
    WORD32 num_min4x4_in_ctb;
    WORD32 slice_qp;
    WORD32 slice_start_ctb_idx;
    WORD32 tile_start_ctb_idx;


    ps_slice_hdr = ps_codec->s_parse.ps_slice_hdr_base;
    ps_pps = ps_codec->s_parse.ps_pps_base;
    ps_sps = ps_codec->s_parse.ps_sps_base;

 /* Get current slice header, pps and sps */
    ps_slice_hdr += (ps_codec->s_parse.i4_cur_slice_idx & (MAX_SLICE_HDR_CNT - 1));
    ps_pps  += ps_slice_hdr->i1_pps_id;
    ps_sps  += ps_pps->i1_sps_id;

 if(0 != ps_codec->s_parse.i4_cur_slice_idx)
 {
 if(!ps_slice_hdr->i1_dependent_slice_flag)
 {
            ps_codec->s_parse.i4_cur_independent_slice_idx++;
 if(MAX_SLICE_HDR_CNT == ps_codec->s_parse.i4_cur_independent_slice_idx)
                ps_codec->s_parse.i4_cur_independent_slice_idx = 0;
 }
 }


    ctb_size = 1 << ps_sps->i1_log2_ctb_size;
    num_min4x4_in_ctb = (ctb_size / 4) * (ctb_size / 4);
    num_ctb_in_row = ps_sps->i2_pic_wd_in_ctb;

 /* Update the parse context */
 if(0 == ps_codec->i4_slice_error)
 {
        ps_codec->s_parse.i4_ctb_x = ps_slice_hdr->i2_ctb_x;
        ps_codec->s_parse.i4_ctb_y = ps_slice_hdr->i2_ctb_y;
 }
    ps_codec->s_parse.ps_pps = ps_pps;
    ps_codec->s_parse.ps_sps = ps_sps;
    ps_codec->s_parse.ps_slice_hdr = ps_slice_hdr;

 /* Derive Tile positions for the current CTB */
 /* Change this to lookup if required */
    ihevcd_get_tile_pos(ps_pps, ps_sps, ps_codec->s_parse.i4_ctb_x,
                        ps_codec->s_parse.i4_ctb_y,
 &ps_codec->s_parse.i4_ctb_tile_x,
 &ps_codec->s_parse.i4_ctb_tile_y,
 &tile_idx);
    ps_codec->s_parse.ps_tile = ps_pps->ps_tile + tile_idx;
    ps_codec->s_parse.i4_cur_tile_idx = tile_idx;
    ps_tile = ps_codec->s_parse.ps_tile;
 if(tile_idx)
        ps_tile_prev = ps_tile - 1;
 else
        ps_tile_prev = ps_tile;

 /* If the present slice is dependent, then store the previous
     * independent slices' ctb x and y values for decoding process */
 if(0 == ps_codec->i4_slice_error)
 {
 if(1 == ps_slice_hdr->i1_dependent_slice_flag)
 {
 /*If slice is present at the start of a new tile*/
 if((0 == ps_codec->s_parse.i4_ctb_tile_x) && (0 == ps_codec->s_parse.i4_ctb_tile_y))
 {
                ps_codec->s_parse.i4_ctb_slice_x = 0;
                ps_codec->s_parse.i4_ctb_slice_y = 0;
 }
 }

 if(!ps_slice_hdr->i1_dependent_slice_flag)
 {
            ps_codec->s_parse.i4_ctb_slice_x = 0;
            ps_codec->s_parse.i4_ctb_slice_y = 0;
 }
 }

 /* Frame level initializations */
 if((0 == ps_codec->s_parse.i4_ctb_y) &&
 (0 == ps_codec->s_parse.i4_ctb_x))
 {
        ret = ihevcd_parse_pic_init(ps_codec);
        RETURN_IF((ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS), ret);

        ps_codec->s_parse.pu4_pic_tu_idx[0] = 0;
        ps_codec->s_parse.pu4_pic_pu_idx[0] = 0;
        ps_codec->s_parse.i4_cur_independent_slice_idx = 0;
        ps_codec->s_parse.i4_ctb_tile_x = 0;
        ps_codec->s_parse.i4_ctb_tile_y = 0;
 }

 {
 /* Updating the poc list of current slice to ps_mv_buf */
 mv_buf_t *ps_mv_buf = ps_codec->s_parse.ps_cur_mv_buf;

 if(ps_slice_hdr->i1_num_ref_idx_l1_active != 0)
 {
 for(i = 0; i < ps_slice_hdr->i1_num_ref_idx_l1_active; i++)
 {
                ps_mv_buf->l1_collocated_poc[(ps_codec->s_parse.i4_cur_slice_idx & (MAX_SLICE_HDR_CNT - 1))][i] = ((pic_buf_t *)ps_slice_hdr->as_ref_pic_list1[i].pv_pic_buf)->i4_abs_poc;
                ps_mv_buf->u1_l1_collocated_poc_lt[(ps_codec->s_parse.i4_cur_slice_idx & (MAX_SLICE_HDR_CNT - 1))][i] = ((pic_buf_t *)ps_slice_hdr->as_ref_pic_list1[i].pv_pic_buf)->u1_used_as_ref;
 }
 }

 if(ps_slice_hdr->i1_num_ref_idx_l0_active != 0)
 {
 for(i = 0; i < ps_slice_hdr->i1_num_ref_idx_l0_active; i++)
 {
                ps_mv_buf->l0_collocated_poc[(ps_codec->s_parse.i4_cur_slice_idx & (MAX_SLICE_HDR_CNT - 1))][i] = ((pic_buf_t *)ps_slice_hdr->as_ref_pic_list0[i].pv_pic_buf)->i4_abs_poc;
                ps_mv_buf->u1_l0_collocated_poc_lt[(ps_codec->s_parse.i4_cur_slice_idx & (MAX_SLICE_HDR_CNT - 1))][i] = ((pic_buf_t *)ps_slice_hdr->as_ref_pic_list0[i].pv_pic_buf)->u1_used_as_ref;
 }
 }
 }

 /*Initialize the low delay flag at the beginning of every slice*/
 if((0 == ps_codec->s_parse.i4_ctb_slice_x) || (0 == ps_codec->s_parse.i4_ctb_slice_y))
 {
 /* Lowdelay flag */
        WORD32 cur_poc, ref_list_poc, flag = 1;
        cur_poc = ps_slice_hdr->i4_abs_pic_order_cnt;
 for(i = 0; i < ps_slice_hdr->i1_num_ref_idx_l0_active; i++)
 {
            ref_list_poc = ((mv_buf_t *)ps_slice_hdr->as_ref_pic_list0[i].pv_mv_buf)->i4_abs_poc;
 if(ref_list_poc > cur_poc)
 {
                flag = 0;
 break;
 }
 }
 if(flag && (ps_slice_hdr->i1_slice_type == BSLICE))
 {
 for(i = 0; i < ps_slice_hdr->i1_num_ref_idx_l1_active; i++)
 {
                ref_list_poc = ((mv_buf_t *)ps_slice_hdr->as_ref_pic_list1[i].pv_mv_buf)->i4_abs_poc;
 if(ref_list_poc > cur_poc)
 {
                    flag = 0;
 break;
 }
 }
 }
        ps_slice_hdr->i1_low_delay_flag = flag;
 }

 /* initialize the cabac init idc based on slice type */
 if(ps_slice_hdr->i1_slice_type == ISLICE)
 {
        cabac_init_idc = 0;
 }
 else if(ps_slice_hdr->i1_slice_type == PSLICE)
 {
        cabac_init_idc = ps_slice_hdr->i1_cabac_init_flag ? 2 : 1;
 }
 else
 {
        cabac_init_idc = ps_slice_hdr->i1_cabac_init_flag ? 1 : 2;
 }

    slice_qp = ps_slice_hdr->i1_slice_qp_delta + ps_pps->i1_pic_init_qp;
    slice_qp = CLIP3(slice_qp, 0, 51);

 /*Update QP value for every indepndent slice or for every dependent slice that begins at the start of a new tile*/
 if((0 == ps_slice_hdr->i1_dependent_slice_flag) ||
 ((1 == ps_slice_hdr->i1_dependent_slice_flag) && ((0 == ps_codec->s_parse.i4_ctb_tile_x) && (0 == ps_codec->s_parse.i4_ctb_tile_y))))
 {
        ps_codec->s_parse.u4_qp = slice_qp;
 }

 /*Cabac init at the beginning of a slice*/
 if((1 == ps_slice_hdr->i1_dependent_slice_flag) && (!((ps_codec->s_parse.i4_ctb_tile_x == 0) && (ps_codec->s_parse.i4_ctb_tile_y == 0))))
 {
 if((0 == ps_pps->i1_entropy_coding_sync_enabled_flag) || (ps_pps->i1_entropy_coding_sync_enabled_flag && (0 != ps_codec->s_parse.i4_ctb_x)))
 {
            ihevcd_cabac_reset(&ps_codec->s_parse.s_cabac,
 &ps_codec->s_parse.s_bitstrm);
 }
 }
 else if((0 == ps_pps->i1_entropy_coding_sync_enabled_flag) || (ps_pps->i1_entropy_coding_sync_enabled_flag && (0 != ps_codec->s_parse.i4_ctb_x)))
 {
        ret = ihevcd_cabac_init(&ps_codec->s_parse.s_cabac,
 &ps_codec->s_parse.s_bitstrm,
                                slice_qp,
                                cabac_init_idc,
 &gau1_ihevc_cab_ctxts[cabac_init_idc][slice_qp][0]);
 if(ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS)
 {
            ps_codec->i4_slice_error = 1;
            end_of_slice_flag = 1;
            ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
 }
 }


 do
 {

 {
            WORD32 cur_ctb_idx = ps_codec->s_parse.i4_ctb_x
 + ps_codec->s_parse.i4_ctb_y * (ps_sps->i2_pic_wd_in_ctb);
 if(1 == ps_codec->i4_num_cores && 0 == cur_ctb_idx % RESET_TU_BUF_NCTB)
 {
                ps_codec->s_parse.ps_tu = ps_codec->s_parse.ps_pic_tu;
                ps_codec->s_parse.i4_pic_tu_idx = 0;
 }
 }

        end_of_pic = 0;
 /* Section:7.3.7 Coding tree unit syntax */
 /* coding_tree_unit() inlined here */
 /* If number of cores is greater than 1, then add job to the queue */
 /* At the start of ctb row parsing in a tile, queue a job for processing the current tile row */
        ps_codec->s_parse.i4_ctb_num_pcm_blks = 0;


 /*At the beginning of each tile-which is not the beginning of a slice, cabac context must be initialized.
         * Hence, check for the tile beginning here */
 if(((0 == ps_codec->s_parse.i4_ctb_tile_x) && (0 == ps_codec->s_parse.i4_ctb_tile_y))
 && (!((ps_tile->u1_pos_x == 0) && (ps_tile->u1_pos_y == 0)))
 && (!((0 == ps_codec->s_parse.i4_ctb_slice_x) && (0 == ps_codec->s_parse.i4_ctb_slice_y))))
 {
            slice_qp = ps_slice_hdr->i1_slice_qp_delta + ps_pps->i1_pic_init_qp;
            slice_qp = CLIP3(slice_qp, 0, 51);
            ps_codec->s_parse.u4_qp = slice_qp;

            ihevcd_get_tile_pos(ps_pps, ps_sps, ps_codec->s_parse.i4_ctb_x,
                                ps_codec->s_parse.i4_ctb_y,
 &ps_codec->s_parse.i4_ctb_tile_x,
 &ps_codec->s_parse.i4_ctb_tile_y,
 &tile_idx);

            ps_codec->s_parse.ps_tile = ps_pps->ps_tile + tile_idx;
            ps_codec->s_parse.i4_cur_tile_idx = tile_idx;
            ps_tile_prev = ps_tile - 1;

            tile_start_ctb_idx = ps_tile->u1_pos_x
 + ps_tile->u1_pos_y * (ps_sps->i2_pic_wd_in_ctb);

            slice_start_ctb_idx =  ps_slice_hdr->i2_ctb_x
 + ps_slice_hdr->i2_ctb_y * (ps_sps->i2_pic_wd_in_ctb);

 /*For slices that span across multiple tiles*/
 if(slice_start_ctb_idx < tile_start_ctb_idx)
 { /* 2 Cases
             * 1 - slice spans across frame-width- but does not start from 1st column
             * 2 - Slice spans across multiple tiles anywhere is a frame
             */
                ps_codec->s_parse.i4_ctb_slice_y = ps_tile->u1_pos_y - ps_slice_hdr->i2_ctb_y;
 if(!(((ps_slice_hdr->i2_ctb_x + ps_tile_prev->u2_wd) % ps_sps->i2_pic_wd_in_ctb) == ps_tile->u1_pos_x)) //Case 2
 {
 if(ps_slice_hdr->i2_ctb_y <= ps_tile->u1_pos_y)
 {
 if(ps_slice_hdr->i2_ctb_x > ps_tile->u1_pos_x)
 {
                            ps_codec->s_parse.i4_ctb_slice_y -= 1;
 }
 }
 }
 /*ps_codec->s_parse.i4_ctb_slice_y = ps_tile->u1_pos_y - ps_slice_hdr->i2_ctb_y;
                if (ps_slice_hdr->i2_ctb_y <= ps_tile->u1_pos_y)
                {
                    if (ps_slice_hdr->i2_ctb_x > ps_tile->u1_pos_x )
                    {
                        ps_codec->s_parse.i4_ctb_slice_y -= 1 ;
                    }
                }*/
 }

 if(!ps_slice_hdr->i1_dependent_slice_flag)
 {
                ret = ihevcd_cabac_init(&ps_codec->s_parse.s_cabac,
 &ps_codec->s_parse.s_bitstrm,
                                        slice_qp,
                                        cabac_init_idc,
 &gau1_ihevc_cab_ctxts[cabac_init_idc][slice_qp][0]);
 if(ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS)
 {
                    ps_codec->i4_slice_error = 1;
                    end_of_slice_flag = 1;
                    ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
 }

 }
 }
 /* If number of cores is greater than 1, then add job to the queue */
 /* At the start of ctb row parsing in a tile, queue a job for processing the current tile row */

 if(0 == ps_codec->s_parse.i4_ctb_tile_x)
 {

 if(1 < ps_codec->i4_num_cores)
 {
 proc_job_t s_job;
                IHEVCD_ERROR_T ret;
                s_job.i4_cmd    = CMD_PROCESS;
                s_job.i2_ctb_cnt = (WORD16)ps_tile->u2_wd;
                s_job.i2_ctb_x = (WORD16)ps_codec->s_parse.i4_ctb_x;
                s_job.i2_ctb_y = (WORD16)ps_codec->s_parse.i4_ctb_y;
                s_job.i2_slice_idx = (WORD16)ps_codec->s_parse.i4_cur_slice_idx;
                s_job.i4_tu_coeff_data_ofst = (UWORD8 *)ps_codec->s_parse.pv_tu_coeff_data -
 (UWORD8 *)ps_codec->s_parse.pv_pic_tu_coeff_data;
                ret = ihevcd_jobq_queue((jobq_t *)ps_codec->s_parse.pv_proc_jobq, &s_job, sizeof(proc_job_t), 1);

 if(ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS)
 return ret;
 }
 else
 {
 process_ctxt_t *ps_proc = &ps_codec->as_process[0];
                WORD32 tu_coeff_data_ofst = (UWORD8 *)ps_codec->s_parse.pv_tu_coeff_data -
 (UWORD8 *)ps_codec->s_parse.pv_pic_tu_coeff_data;

 /* If the codec is running in single core mode,
                 * initialize zeroth process context
                 * TODO: Dual core mode might need a different implementation instead of jobq
                 */

                ps_proc->i4_ctb_cnt = ps_tile->u2_wd;
                ps_proc->i4_ctb_x   = ps_codec->s_parse.i4_ctb_x;
                ps_proc->i4_ctb_y   = ps_codec->s_parse.i4_ctb_y;
                ps_proc->i4_cur_slice_idx = ps_codec->s_parse.i4_cur_slice_idx;

                ihevcd_init_proc_ctxt(ps_proc, tu_coeff_data_ofst);
 }
 }


 /* Restore cabac context model from top right CTB if entropy sync is enabled */
 if(ps_pps->i1_entropy_coding_sync_enabled_flag)
 {
 /*TODO Handle single CTB and top-right belonging to a different slice */
 if(0 == ps_codec->s_parse.i4_ctb_x)
 {
                WORD32 default_ctxt = 0;

 if((0 == ps_codec->s_parse.i4_ctb_slice_y) && (!ps_slice_hdr->i1_dependent_slice_flag))
                    default_ctxt = 1;
 if(1 == ps_sps->i2_pic_wd_in_ctb)
                    default_ctxt = 1;

                ps_codec->s_parse.u4_qp = slice_qp;
 if(default_ctxt)
 {
                    ret = ihevcd_cabac_init(&ps_codec->s_parse.s_cabac,
 &ps_codec->s_parse.s_bitstrm,
                                            slice_qp,
                                            cabac_init_idc,
 &gau1_ihevc_cab_ctxts[cabac_init_idc][slice_qp][0]);

 if(ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS)
 {
                        ps_codec->i4_slice_error = 1;
                        end_of_slice_flag = 1;
                        ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
 }
 }
 else
 {
                    ret = ihevcd_cabac_init(&ps_codec->s_parse.s_cabac,
 &ps_codec->s_parse.s_bitstrm,
                                            slice_qp,
                                            cabac_init_idc,
 (const UWORD8 *)&ps_codec->s_parse.s_cabac.au1_ctxt_models_sync);

 if(ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS)
 {
                        ps_codec->i4_slice_error = 1;
                        end_of_slice_flag = 1;
                        ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
 }
 }
 }
 }



 if(0 == ps_codec->i4_slice_error)
 {
 if(ps_slice_hdr->i1_slice_sao_luma_flag || ps_slice_hdr->i1_slice_sao_chroma_flag)
                ihevcd_parse_sao(ps_codec);
 }
 else
 {
 sao_t *ps_sao = ps_codec->s_parse.ps_pic_sao +
                            ps_codec->s_parse.i4_ctb_x +
                            ps_codec->s_parse.i4_ctb_y * ps_sps->i2_pic_wd_in_ctb;

 /* Default values */
            ps_sao->b3_y_type_idx = 0;
            ps_sao->b3_cb_type_idx = 0;
            ps_sao->b3_cr_type_idx = 0;
 }


 {
            WORD32 ctb_indx;
            ctb_indx = ps_codec->s_parse.i4_ctb_x + ps_sps->i2_pic_wd_in_ctb * ps_codec->s_parse.i4_ctb_y;
            ps_codec->s_parse.s_bs_ctxt.pu1_pic_qp_const_in_ctb[ctb_indx >> 3] |= (1 << (ctb_indx & 7));
 {
                UWORD16 *pu1_slice_idx = ps_codec->s_parse.pu1_slice_idx;
                pu1_slice_idx[ctb_indx] = ps_codec->s_parse.i4_cur_independent_slice_idx;
 }
 }

 if(0 == ps_codec->i4_slice_error)
 {
 tu_t *ps_tu = ps_codec->s_parse.ps_tu;
            WORD32 i4_tu_cnt = ps_codec->s_parse.s_cu.i4_tu_cnt;
            WORD32 i4_pic_tu_idx = ps_codec->s_parse.i4_pic_tu_idx;

 pu_t *ps_pu = ps_codec->s_parse.ps_pu;
            WORD32 i4_pic_pu_idx = ps_codec->s_parse.i4_pic_pu_idx;

            UWORD8 *pu1_tu_coeff_data = (UWORD8 *)ps_codec->s_parse.pv_tu_coeff_data;

            ret = ihevcd_parse_coding_quadtree(ps_codec,
 (ps_codec->s_parse.i4_ctb_x << ps_sps->i1_log2_ctb_size),
 (ps_codec->s_parse.i4_ctb_y << ps_sps->i1_log2_ctb_size),
                                               ps_sps->i1_log2_ctb_size,
 0);
 /* Check for error */

             if (ret != (IHEVCD_ERROR_T)IHEVCD_SUCCESS)
             {
                 /* Reset tu and pu parameters, and signal current ctb as skip */
                 WORD32 tu_coeff_data_reset_size;
 
                 ps_codec->s_parse.ps_tu = ps_tu;
                 ps_codec->s_parse.s_cu.i4_tu_cnt = i4_tu_cnt;
                 ps_codec->s_parse.i4_pic_tu_idx = i4_pic_tu_idx;


                 ps_codec->s_parse.ps_pu = ps_pu;
                 ps_codec->s_parse.i4_pic_pu_idx = i4_pic_pu_idx;
 
                 tu_coeff_data_reset_size = (UWORD8 *)ps_codec->s_parse.pv_tu_coeff_data - pu1_tu_coeff_data;
                 memset(pu1_tu_coeff_data, 0, tu_coeff_data_reset_size);
                 ps_codec->s_parse.pv_tu_coeff_data = (void *)pu1_tu_coeff_data;
 
                ihevcd_set_ctb_skip(ps_codec);
 
                 /* Set slice error to suppress further parsing and
                  * signal end of slice.
                 */
                ps_codec->i4_slice_error = 1;
                end_of_slice_flag = 1;
                ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
 }

         }
         else
         {
            ihevcd_set_ctb_skip(ps_codec);
         }
 
         if(0 == ps_codec->i4_slice_error)
            end_of_slice_flag = ihevcd_cabac_decode_terminate(&ps_codec->s_parse.s_cabac, &ps_codec->s_parse.s_bitstrm);

        AEV_TRACE("end_of_slice_flag", end_of_slice_flag, ps_codec->s_parse.s_cabac.u4_range);


 /* In case of tiles or entropy sync, terminate cabac and copy cabac context backed up at the end of top-right CTB */
 if(ps_pps->i1_tiles_enabled_flag || ps_pps->i1_entropy_coding_sync_enabled_flag)
 {
            WORD32 end_of_tile = 0;
            WORD32 end_of_tile_row = 0;

 /* Take a back up of cabac context models if entropy sync is enabled */
 if(ps_pps->i1_entropy_coding_sync_enabled_flag || ps_pps->i1_tiles_enabled_flag)
 {
 if(1 == ps_codec->s_parse.i4_ctb_x)
 {
                    WORD32 size = sizeof(ps_codec->s_parse.s_cabac.au1_ctxt_models);
                    memcpy(&ps_codec->s_parse.s_cabac.au1_ctxt_models_sync, &ps_codec->s_parse.s_cabac.au1_ctxt_models, size);
 }
 }

 /* Since tiles and entropy sync are not enabled simultaneously, the following will not result in any problems */
 if((ps_codec->s_parse.i4_ctb_tile_x + 1) == (ps_tile->u2_wd))
 {
                end_of_tile_row = 1;
 if((ps_codec->s_parse.i4_ctb_tile_y + 1) == ps_tile->u2_ht)
                    end_of_tile = 1;
 }
 if((0 == end_of_slice_flag) &&
 ((ps_pps->i1_tiles_enabled_flag && end_of_tile) ||
 (ps_pps->i1_entropy_coding_sync_enabled_flag && end_of_tile_row)))
 {
                WORD32 end_of_sub_stream_one_bit;
                end_of_sub_stream_one_bit = ihevcd_cabac_decode_terminate(&ps_codec->s_parse.s_cabac, &ps_codec->s_parse.s_bitstrm);
                AEV_TRACE("end_of_sub_stream_one_bit", end_of_sub_stream_one_bit, ps_codec->s_parse.s_cabac.u4_range);

 /* TODO: Remove the check for offset when HM is updated to include a byte unconditionally even for aligned location */
 /* For Ittiam streams this check should not be there, for HM9.1 streams this should be there */
 if(ps_codec->s_parse.s_bitstrm.u4_bit_ofst % 8)
                    ihevcd_bits_flush_to_byte_boundary(&ps_codec->s_parse.s_bitstrm);

                UNUSED(end_of_sub_stream_one_bit);
 }
 }
 {
            WORD32 ctb_indx;

            ctb_addr = ps_codec->s_parse.i4_ctb_y * num_ctb_in_row + ps_codec->s_parse.i4_ctb_x;

            ctb_indx = ++ctb_addr;

 /* Store pu_idx for next CTB in frame level pu_idx array */

 if((ps_tile->u2_wd == (ps_codec->s_parse.i4_ctb_tile_x + 1)) && (ps_tile->u2_wd != ps_sps->i2_pic_wd_in_ctb))
 {
                ctb_indx = (ps_sps->i2_pic_wd_in_ctb * (ps_codec->s_parse.i4_ctb_tile_y + 1 + ps_tile->u1_pos_y)) + ps_tile->u1_pos_x; //idx is the beginning of next row in current tile.
 if(ps_tile->u2_ht == (ps_codec->s_parse.i4_ctb_tile_y + 1))
 {
 if((ps_tile->u2_wd + ps_tile->u1_pos_x == ps_sps->i2_pic_wd_in_ctb) && ((ps_tile->u2_ht + ps_tile->u1_pos_y == ps_sps->i2_pic_ht_in_ctb)))
 {
                        ctb_indx = ctb_addr; //Next continuous ctb address
 }
 else //Not last tile's end , but a tile end
 {
 tile_t *ps_next_tile = ps_codec->s_parse.ps_tile + 1;
                        ctb_indx = ps_next_tile->u1_pos_x + (ps_next_tile->u1_pos_y * ps_sps->i2_pic_wd_in_ctb); //idx is the beginning of first row in next tile.
 }
 }
 }

            ps_codec->s_parse.pu4_pic_pu_idx[ctb_indx] = ps_codec->s_parse.i4_pic_pu_idx;
            ps_codec->s_parse.i4_next_pu_ctb_cnt = ctb_indx;

            ps_codec->s_parse.pu1_pu_map += num_min4x4_in_ctb;

 /* Store tu_idx for next CTB in frame level tu_idx array */
 if(1 == ps_codec->i4_num_cores)
 {
                ctb_indx = (0 == ctb_addr % RESET_TU_BUF_NCTB) ?
                                RESET_TU_BUF_NCTB : ctb_addr % RESET_TU_BUF_NCTB;

 if((ps_tile->u2_wd == (ps_codec->s_parse.i4_ctb_tile_x + 1)) && (ps_tile->u2_wd != ps_sps->i2_pic_wd_in_ctb))
 {
                    ctb_indx = (ps_sps->i2_pic_wd_in_ctb * (ps_codec->s_parse.i4_ctb_tile_y + 1 + ps_tile->u1_pos_y)) + ps_tile->u1_pos_x; //idx is the beginning of next row in current tile.
 if(ps_tile->u2_ht == (ps_codec->s_parse.i4_ctb_tile_y + 1))
 {
 if((ps_tile->u2_wd + ps_tile->u1_pos_x == ps_sps->i2_pic_wd_in_ctb) && ((ps_tile->u2_ht + ps_tile->u1_pos_y == ps_sps->i2_pic_ht_in_ctb)))
 {
                            ctb_indx = (0 == ctb_addr % RESET_TU_BUF_NCTB) ?
                                            RESET_TU_BUF_NCTB : ctb_addr % RESET_TU_BUF_NCTB;
 }
 else //Not last tile's end , but a tile end
 {
 tile_t *ps_next_tile = ps_codec->s_parse.ps_tile + 1;
                            ctb_indx =  ps_next_tile->u1_pos_x + (ps_next_tile->u1_pos_y * ps_sps->i2_pic_wd_in_ctb); //idx is the beginning of first row in next tile.
 }
 }
 }
                ps_codec->s_parse.i4_next_tu_ctb_cnt = ctb_indx;
                ps_codec->s_parse.pu4_pic_tu_idx[ctb_indx] = ps_codec->s_parse.i4_pic_tu_idx;
 }
 else
 {
                ctb_indx = ctb_addr;
 if((ps_tile->u2_wd == (ps_codec->s_parse.i4_ctb_tile_x + 1)) && (ps_tile->u2_wd != ps_sps->i2_pic_wd_in_ctb))
 {
                    ctb_indx = (ps_sps->i2_pic_wd_in_ctb * (ps_codec->s_parse.i4_ctb_tile_y + 1 + ps_tile->u1_pos_y)) + ps_tile->u1_pos_x; //idx is the beginning of next row in current tile.
 if(ps_tile->u2_ht == (ps_codec->s_parse.i4_ctb_tile_y + 1))
 {
 if((ps_tile->u2_wd + ps_tile->u1_pos_x == ps_sps->i2_pic_wd_in_ctb) && ((ps_tile->u2_ht + ps_tile->u1_pos_y == ps_sps->i2_pic_ht_in_ctb)))
 {
                            ctb_indx = ctb_addr;
 }
 else //Not last tile's end , but a tile end
 {
 tile_t *ps_next_tile = ps_codec->s_parse.ps_tile + 1;
                            ctb_indx =  ps_next_tile->u1_pos_x + (ps_next_tile->u1_pos_y * ps_sps->i2_pic_wd_in_ctb); //idx is the beginning of first row in next tile.
 }
 }
 }
                ps_codec->s_parse.i4_next_tu_ctb_cnt = ctb_indx;
                ps_codec->s_parse.pu4_pic_tu_idx[ctb_indx] = ps_codec->s_parse.i4_pic_tu_idx;
 }
            ps_codec->s_parse.pu1_tu_map += num_min4x4_in_ctb;
 }


 if(ps_codec->i4_num_cores <= MV_PRED_NUM_CORES_THRESHOLD)
 {
 /*************************************************/
 /****************   MV pred **********************/
 /*************************************************/
            WORD8 u1_top_ctb_avail = 1;
            WORD8 u1_left_ctb_avail = 1;
            WORD8 u1_top_lt_ctb_avail = 1;
            WORD8 u1_top_rt_ctb_avail = 1;
            WORD16 i2_wd_in_ctb;

            tile_start_ctb_idx = ps_tile->u1_pos_x
 + ps_tile->u1_pos_y * (ps_sps->i2_pic_wd_in_ctb);

            slice_start_ctb_idx =  ps_slice_hdr->i2_ctb_x
 + ps_slice_hdr->i2_ctb_y * (ps_sps->i2_pic_wd_in_ctb);

 if((slice_start_ctb_idx < tile_start_ctb_idx))
 {
                i2_wd_in_ctb = ps_sps->i2_pic_wd_in_ctb;
 }
 else
 {
                i2_wd_in_ctb = ps_tile->u2_wd;
 }
 /* slice and tile boundaries */
 if((0 == ps_codec->s_parse.i4_ctb_y) || (0 == ps_codec->s_parse.i4_ctb_tile_y))
 {
                u1_top_ctb_avail = 0;
                u1_top_lt_ctb_avail = 0;
                u1_top_rt_ctb_avail = 0;
 }

 if((0 == ps_codec->s_parse.i4_ctb_x) || (0 == ps_codec->s_parse.i4_ctb_tile_x))
 {
                u1_left_ctb_avail = 0;
                u1_top_lt_ctb_avail = 0;
 if((0 == ps_codec->s_parse.i4_ctb_slice_y) || (0 == ps_codec->s_parse.i4_ctb_tile_y))
 {
                    u1_top_ctb_avail = 0;
 if((i2_wd_in_ctb - 1) != ps_codec->s_parse.i4_ctb_slice_x) //TODO: For tile, not implemented
 {
                        u1_top_rt_ctb_avail = 0;
 }
 }
 }
 /*For slices not beginning at start of a ctb row*/
 else if(ps_codec->s_parse.i4_ctb_x > 0)
 {
 if((0 == ps_codec->s_parse.i4_ctb_slice_y) || (0 == ps_codec->s_parse.i4_ctb_tile_y))
 {
                    u1_top_ctb_avail = 0;
                    u1_top_lt_ctb_avail = 0;
 if(0 == ps_codec->s_parse.i4_ctb_slice_x)
 {
                        u1_left_ctb_avail = 0;
 }
 if((i2_wd_in_ctb - 1) != ps_codec->s_parse.i4_ctb_slice_x)
 {
                        u1_top_rt_ctb_avail = 0;
 }
 }
 else if((1 == ps_codec->s_parse.i4_ctb_slice_y) && (0 == ps_codec->s_parse.i4_ctb_slice_x))
 {
                    u1_top_lt_ctb_avail = 0;
 }
 }

 if(((ps_sps->i2_pic_wd_in_ctb - 1) == ps_codec->s_parse.i4_ctb_x) || ((ps_tile->u2_wd - 1) == ps_codec->s_parse.i4_ctb_tile_x))
 {
                u1_top_rt_ctb_avail = 0;
 }

 if(PSLICE == ps_slice_hdr->i1_slice_type
 || BSLICE == ps_slice_hdr->i1_slice_type)
 {
 mv_ctxt_t s_mv_ctxt;
 process_ctxt_t *ps_proc;
                UWORD32 *pu4_ctb_top_pu_idx;
                UWORD32 *pu4_ctb_left_pu_idx;
                UWORD32 *pu4_ctb_top_left_pu_idx;
                WORD32 i4_ctb_pu_cnt;
                WORD32 cur_ctb_idx;
                WORD32 next_ctb_idx;
                WORD32 cur_pu_idx;
                ps_proc = &ps_codec->as_process[(ps_codec->i4_num_cores == 1) ? 1 : (ps_codec->i4_num_cores - 1)];
                cur_ctb_idx = ps_codec->s_parse.i4_ctb_x
 + ps_codec->s_parse.i4_ctb_y * (ps_sps->i2_pic_wd_in_ctb);
                next_ctb_idx = ps_codec->s_parse.i4_next_pu_ctb_cnt;
                i4_ctb_pu_cnt = ps_codec->s_parse.pu4_pic_pu_idx[next_ctb_idx]
 - ps_codec->s_parse.pu4_pic_pu_idx[cur_ctb_idx];

                cur_pu_idx = ps_codec->s_parse.pu4_pic_pu_idx[cur_ctb_idx];

                pu4_ctb_top_pu_idx = ps_proc->pu4_pic_pu_idx_top
 + (ps_codec->s_parse.i4_ctb_x * ctb_size / MIN_PU_SIZE);
                pu4_ctb_left_pu_idx = ps_proc->pu4_pic_pu_idx_left;
                pu4_ctb_top_left_pu_idx = &ps_proc->u4_ctb_top_left_pu_idx;

 /* Initializing s_mv_ctxt */
 {
                    s_mv_ctxt.ps_pps = ps_pps;
                    s_mv_ctxt.ps_sps = ps_sps;
                    s_mv_ctxt.ps_slice_hdr = ps_slice_hdr;
                    s_mv_ctxt.i4_ctb_x = ps_codec->s_parse.i4_ctb_x;
                    s_mv_ctxt.i4_ctb_y = ps_codec->s_parse.i4_ctb_y;
                    s_mv_ctxt.ps_pu = &ps_codec->s_parse.ps_pic_pu[cur_pu_idx];
                    s_mv_ctxt.ps_pic_pu = ps_codec->s_parse.ps_pic_pu;
                    s_mv_ctxt.ps_tile = ps_tile;
                    s_mv_ctxt.pu4_pic_pu_idx_map = ps_proc->pu4_pic_pu_idx_map;
                    s_mv_ctxt.pu4_pic_pu_idx = ps_codec->s_parse.pu4_pic_pu_idx;
                    s_mv_ctxt.pu1_pic_pu_map = ps_codec->s_parse.pu1_pic_pu_map;
                    s_mv_ctxt.i4_ctb_pu_cnt = i4_ctb_pu_cnt;
                    s_mv_ctxt.i4_ctb_start_pu_idx = cur_pu_idx;
                    s_mv_ctxt.u1_top_ctb_avail = u1_top_ctb_avail;
                    s_mv_ctxt.u1_top_rt_ctb_avail = u1_top_rt_ctb_avail;
                    s_mv_ctxt.u1_top_lt_ctb_avail = u1_top_lt_ctb_avail;
                    s_mv_ctxt.u1_left_ctb_avail = u1_left_ctb_avail;
 }

                ihevcd_get_mv_ctb(&s_mv_ctxt, pu4_ctb_top_pu_idx,
                                  pu4_ctb_left_pu_idx, pu4_ctb_top_left_pu_idx);

 }
 else
 {
                WORD32 num_minpu_in_ctb = (ctb_size / MIN_PU_SIZE) * (ctb_size / MIN_PU_SIZE);
                UWORD8 *pu1_pic_pu_map_ctb = ps_codec->s_parse.pu1_pic_pu_map +
 (ps_codec->s_parse.i4_ctb_x + ps_codec->s_parse.i4_ctb_y * ps_sps->i2_pic_wd_in_ctb) * num_minpu_in_ctb;
 process_ctxt_t *ps_proc = &ps_codec->as_process[(ps_codec->i4_num_cores == 1) ? 1 : (ps_codec->i4_num_cores - 1)];
                WORD32 row, col;
                WORD32 pu_cnt;
                WORD32 num_pu_per_ctb;
                WORD32 cur_ctb_idx;
                WORD32 next_ctb_idx;
                WORD32 ctb_start_pu_idx;
                UWORD32 *pu4_nbr_pu_idx = ps_proc->pu4_pic_pu_idx_map;
                WORD32 nbr_pu_idx_strd = MAX_CTB_SIZE / MIN_PU_SIZE + 2;
 pu_t *ps_pu;

 for(row = 0; row < ctb_size / MIN_PU_SIZE; row++)
 {
 for(col = 0; col < ctb_size / MIN_PU_SIZE; col++)
 {
                        pu1_pic_pu_map_ctb[row * ctb_size / MIN_PU_SIZE + col] = 0;
 }
 }


 /* Neighbor PU idx update inside CTB */
 /* 1byte per 4x4. Indicates the PU idx that 4x4 block belongs to */

                cur_ctb_idx = ps_codec->s_parse.i4_ctb_x
 + ps_codec->s_parse.i4_ctb_y * (ps_sps->i2_pic_wd_in_ctb);
                next_ctb_idx = ps_codec->s_parse.i4_next_pu_ctb_cnt;
                num_pu_per_ctb = ps_codec->s_parse.pu4_pic_pu_idx[next_ctb_idx]
 - ps_codec->s_parse.pu4_pic_pu_idx[cur_ctb_idx];
                ctb_start_pu_idx = ps_codec->s_parse.pu4_pic_pu_idx[cur_ctb_idx];
                ps_pu = &ps_codec->s_parse.ps_pic_pu[ctb_start_pu_idx];

 for(pu_cnt = 0; pu_cnt < num_pu_per_ctb; pu_cnt++, ps_pu++)
 {
                    UWORD32 cur_pu_idx;
                    WORD32 pu_ht = (ps_pu->b4_ht + 1) << 2;
                    WORD32 pu_wd = (ps_pu->b4_wd + 1) << 2;

                    cur_pu_idx = ctb_start_pu_idx + pu_cnt;

 for(row = 0; row < pu_ht / MIN_PU_SIZE; row++)
 for(col = 0; col < pu_wd / MIN_PU_SIZE; col++)
                            pu4_nbr_pu_idx[(1 + ps_pu->b4_pos_x + col)
 + (1 + ps_pu->b4_pos_y + row)
 * nbr_pu_idx_strd] =
                                            cur_pu_idx;
 }

 /* Updating Top and Left pointers */
 {
                    WORD32 rows_remaining = ps_sps->i2_pic_height_in_luma_samples
 - (ps_codec->s_parse.i4_ctb_y << ps_sps->i1_log2_ctb_size);
                    WORD32 ctb_size_left = MIN(ctb_size, rows_remaining);

 /* Top Left */
 /* saving top left before updating top ptr, as updating top ptr will overwrite the top left for the next ctb */
                    ps_proc->u4_ctb_top_left_pu_idx = ps_proc->pu4_pic_pu_idx_top[(ps_codec->s_parse.i4_ctb_x * ctb_size / MIN_PU_SIZE) + ctb_size / MIN_PU_SIZE - 1];
 for(i = 0; i < ctb_size / MIN_PU_SIZE; i++)
 {
 /* Left */
 /* Last column of au4_nbr_pu_idx */
                        ps_proc->pu4_pic_pu_idx_left[i] = pu4_nbr_pu_idx[(ctb_size / MIN_PU_SIZE)
 + (i + 1) * nbr_pu_idx_strd];
 /* Top */
 /* Last row of au4_nbr_pu_idx */
                        ps_proc->pu4_pic_pu_idx_top[(ps_codec->s_parse.i4_ctb_x * ctb_size / MIN_PU_SIZE) + i] =
                                        pu4_nbr_pu_idx[(ctb_size_left / MIN_PU_SIZE) * nbr_pu_idx_strd + i + 1];

 }
 }
 }

 /*************************************************/
 /******************  BS, QP  *********************/
 /*************************************************/
 /* Check if deblock is disabled for the current slice or if it is disabled for the current picture
             * because of disable deblock api
             */
 if(0 == ps_codec->i4_disable_deblk_pic)
 {
 if((0 == ps_slice_hdr->i1_slice_disable_deblocking_filter_flag) &&
 (0 == ps_codec->i4_slice_error))
 {
                    WORD32 i4_ctb_tu_cnt;
                    WORD32 cur_ctb_idx, next_ctb_idx;
                    WORD32 cur_pu_idx;
                    WORD32 cur_tu_idx;
 process_ctxt_t *ps_proc;

                    ps_proc = &ps_codec->as_process[(ps_codec->i4_num_cores == 1) ? 1 : (ps_codec->i4_num_cores - 1)];
                    cur_ctb_idx = ps_codec->s_parse.i4_ctb_x
 + ps_codec->s_parse.i4_ctb_y * (ps_sps->i2_pic_wd_in_ctb);

                    cur_pu_idx = ps_codec->s_parse.pu4_pic_pu_idx[cur_ctb_idx];
                    next_ctb_idx = ps_codec->s_parse.i4_next_tu_ctb_cnt;
 if(1 == ps_codec->i4_num_cores)
 {
                        i4_ctb_tu_cnt = ps_codec->s_parse.pu4_pic_tu_idx[next_ctb_idx] -
                                        ps_codec->s_parse.pu4_pic_tu_idx[cur_ctb_idx % RESET_TU_BUF_NCTB];

                        cur_tu_idx = ps_codec->s_parse.pu4_pic_tu_idx[cur_ctb_idx % RESET_TU_BUF_NCTB];
 }
 else
 {
                        i4_ctb_tu_cnt = ps_codec->s_parse.pu4_pic_tu_idx[next_ctb_idx] -
                                        ps_codec->s_parse.pu4_pic_tu_idx[cur_ctb_idx];

                        cur_tu_idx = ps_codec->s_parse.pu4_pic_tu_idx[cur_ctb_idx];
 }

                    ps_codec->s_parse.s_bs_ctxt.ps_pps = ps_codec->s_parse.ps_pps;
                    ps_codec->s_parse.s_bs_ctxt.ps_sps = ps_codec->s_parse.ps_sps;
                    ps_codec->s_parse.s_bs_ctxt.ps_codec = ps_codec;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_tu_cnt = i4_ctb_tu_cnt;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_x = ps_codec->s_parse.i4_ctb_x;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_y = ps_codec->s_parse.i4_ctb_y;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_tile_x = ps_codec->s_parse.i4_ctb_tile_x;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_tile_y = ps_codec->s_parse.i4_ctb_tile_y;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_slice_x = ps_codec->s_parse.i4_ctb_slice_x;
                    ps_codec->s_parse.s_bs_ctxt.i4_ctb_slice_y = ps_codec->s_parse.i4_ctb_slice_y;
                    ps_codec->s_parse.s_bs_ctxt.ps_tu = &ps_codec->s_parse.ps_pic_tu[cur_tu_idx];
                    ps_codec->s_parse.s_bs_ctxt.ps_pu = &ps_codec->s_parse.ps_pic_pu[cur_pu_idx];
                    ps_codec->s_parse.s_bs_ctxt.pu4_pic_pu_idx_map = ps_proc->pu4_pic_pu_idx_map;
                    ps_codec->s_parse.s_bs_ctxt.i4_next_pu_ctb_cnt = ps_codec->s_parse.i4_next_pu_ctb_cnt;
                    ps_codec->s_parse.s_bs_ctxt.i4_next_tu_ctb_cnt = ps_codec->s_parse.i4_next_tu_ctb_cnt;
                    ps_codec->s_parse.s_bs_ctxt.pu1_slice_idx = ps_codec->s_parse.pu1_slice_idx;
                    ps_codec->s_parse.s_bs_ctxt.ps_slice_hdr = ps_codec->s_parse.ps_slice_hdr;
                    ps_codec->s_parse.s_bs_ctxt.ps_tile = ps_codec->s_parse.ps_tile;

 if(ISLICE == ps_slice_hdr->i1_slice_type)
 {
                        ihevcd_ctb_boundary_strength_islice(&ps_codec->s_parse.s_bs_ctxt);
 }
 else
 {
                        ihevcd_ctb_boundary_strength_pbslice(&ps_codec->s_parse.s_bs_ctxt);
 }
 }
 else
 {
                    WORD32 bs_strd = (ps_sps->i2_pic_wd_in_ctb + 1) * (ctb_size * ctb_size / 8 / 16);

                    UWORD32 *pu4_vert_bs = (UWORD32 *)((UWORD8 *)ps_codec->s_parse.s_bs_ctxt.pu4_pic_vert_bs +
                                    ps_codec->s_parse.i4_ctb_x * (ctb_size * ctb_size / 8 / 16) +
                                    ps_codec->s_parse.i4_ctb_y * bs_strd);
                    UWORD32 *pu4_horz_bs = (UWORD32 *)((UWORD8 *)ps_codec->s_parse.s_bs_ctxt.pu4_pic_horz_bs +
                                    ps_codec->s_parse.i4_ctb_x * (ctb_size * ctb_size / 8 / 16) +
                                    ps_codec->s_parse.i4_ctb_y * bs_strd);

                    memset(pu4_vert_bs, 0, (ctb_size / 8 + 1) * (ctb_size / 4) / 8 * 2);
                    memset(pu4_horz_bs, 0, (ctb_size / 8) * (ctb_size / 4) / 8 * 2);

 }
 }

 }


 /* Update the parse status map */
 {
 sps_t *ps_sps = ps_codec->s_parse.ps_sps;
            UWORD8 *pu1_buf;
            WORD32 idx;
            idx = (ps_codec->s_parse.i4_ctb_x);
            idx += ((ps_codec->s_parse.i4_ctb_y) * ps_sps->i2_pic_wd_in_ctb);
            pu1_buf = (ps_codec->pu1_parse_map + idx);
 *pu1_buf = 1;
 }

 /* Increment CTB x and y positions */
        ps_codec->s_parse.i4_ctb_tile_x++;
        ps_codec->s_parse.i4_ctb_x++;
        ps_codec->s_parse.i4_ctb_slice_x++;

 /*If tiles are enabled, handle the slice counters differently*/
 if(ps_pps->i1_tiles_enabled_flag)
 {
            tile_start_ctb_idx = ps_tile->u1_pos_x
 + ps_tile->u1_pos_y * (ps_sps->i2_pic_wd_in_ctb);

            slice_start_ctb_idx =  ps_slice_hdr->i2_ctb_x
 + ps_slice_hdr->i2_ctb_y * (ps_sps->i2_pic_wd_in_ctb);

 if((slice_start_ctb_idx < tile_start_ctb_idx))
 {
 if(ps_codec->s_parse.i4_ctb_slice_x == (ps_tile->u1_pos_x + ps_tile->u2_wd))
 {
 /* Reached end of slice row within a tile /frame */
                    ps_codec->s_parse.i4_ctb_slice_y++;
                    ps_codec->s_parse.i4_ctb_slice_x = ps_tile->u1_pos_x; //todo:Check
 }
 }
 else if(ps_codec->s_parse.i4_ctb_slice_x == (ps_tile->u2_wd))
 {
                ps_codec->s_parse.i4_ctb_slice_y++;
                ps_codec->s_parse.i4_ctb_slice_x = 0;
 }
 }
 else
 {
 if(ps_codec->s_parse.i4_ctb_slice_x == ps_tile->u2_wd)
 {
 /* Reached end of slice row within a tile /frame */
                ps_codec->s_parse.i4_ctb_slice_y++;
                ps_codec->s_parse.i4_ctb_slice_x = 0;
 }
 }


 if(ps_codec->s_parse.i4_ctb_tile_x == (ps_tile->u2_wd))
 {
 /* Reached end of tile row */
            ps_codec->s_parse.i4_ctb_tile_x = 0;
            ps_codec->s_parse.i4_ctb_x = ps_tile->u1_pos_x;

            ps_codec->s_parse.i4_ctb_tile_y++;
            ps_codec->s_parse.i4_ctb_y++;

 if(ps_codec->s_parse.i4_ctb_tile_y == (ps_tile->u2_ht))
 {
 /* Reached End of Tile */
                ps_codec->s_parse.i4_ctb_tile_y = 0;
                ps_codec->s_parse.i4_ctb_tile_x = 0;
                ps_codec->s_parse.ps_tile++;

 if((ps_tile->u2_ht + ps_tile->u1_pos_y  ==  ps_sps->i2_pic_ht_in_ctb) && (ps_tile->u2_wd + ps_tile->u1_pos_x  ==  ps_sps->i2_pic_wd_in_ctb))
 {
 /* Reached end of frame */
                    end_of_pic = 1;
                    ps_codec->s_parse.i4_ctb_x = 0;
                    ps_codec->s_parse.i4_ctb_y = ps_sps->i2_pic_ht_in_ctb;
 }
 else
 {
 /* Initialize ctb_x and ctb_y to start of next tile */
                    ps_tile = ps_codec->s_parse.ps_tile;
                    ps_codec->s_parse.i4_ctb_x = ps_tile->u1_pos_x;
                    ps_codec->s_parse.i4_ctb_y = ps_tile->u1_pos_y;
                    ps_codec->s_parse.i4_ctb_tile_y = 0;
                    ps_codec->s_parse.i4_ctb_tile_x = 0;
                    ps_codec->s_parse.i4_ctb_slice_x = ps_tile->u1_pos_x;
                    ps_codec->s_parse.i4_ctb_slice_y = ps_tile->u1_pos_y;

 }
 }

 }

        ps_codec->s_parse.i4_next_ctb_indx = ps_codec->s_parse.i4_ctb_x +
                        ps_codec->s_parse.i4_ctb_y * ps_sps->i2_pic_wd_in_ctb;

 /* If the current slice is in error, check if the next slice's address
         * is reached and mark the end_of_slice flag */
 if(ps_codec->i4_slice_error)
 {
 slice_header_t *ps_slice_hdr_next = ps_slice_hdr + 1;
            WORD32 next_slice_addr = ps_slice_hdr_next->i2_ctb_x +
                            ps_slice_hdr_next->i2_ctb_y * ps_sps->i2_pic_wd_in_ctb;

 if(ps_codec->s_parse.i4_next_ctb_indx == next_slice_addr)
                end_of_slice_flag = 1;
 }

 /* If the codec is running in single core mode
         * then call process function for current CTB
         */
 if((1 == ps_codec->i4_num_cores) && (ps_codec->s_parse.i4_ctb_tile_x == 0))
 {
 process_ctxt_t *ps_proc = &ps_codec->as_process[0];
            ps_proc->i4_ctb_cnt = ps_proc->ps_tile->u2_wd;
            ihevcd_process(ps_proc);
 }

 /* If the bytes for the current slice are exhausted
         * set end_of_slice flag to 1
         * This slice will be treated as incomplete */
 if((UWORD8 *)ps_codec->s_parse.s_bitstrm.pu1_buf_max + BITSTRM_OFF_THRS <
 ((UWORD8 *)ps_codec->s_parse.s_bitstrm.pu4_buf + (ps_codec->s_parse.s_bitstrm.u4_bit_ofst / 8)))
 {

 if(0 == ps_codec->i4_slice_error)
                end_of_slice_flag = 1;
 }


 if(end_of_pic)
 break;
 } while(!end_of_slice_flag);

 /* Reset slice error */
    ps_codec->i4_slice_error = 0;

 /* Increment the slice index for parsing next slice */
 if(0 == end_of_pic)
 {
 while(1)
 {

            WORD32 parse_slice_idx;
            parse_slice_idx = ps_codec->s_parse.i4_cur_slice_idx;
            parse_slice_idx++;

 {
 /* If the next slice header is not initialized, update cur_slice_idx and break */
 if((1 == ps_codec->i4_num_cores) || (0 != (parse_slice_idx & (MAX_SLICE_HDR_CNT - 1))))
 {
                    ps_codec->s_parse.i4_cur_slice_idx = parse_slice_idx;
 break;
 }

 /* If the next slice header is initialised, wait for the parsed slices to be processed */
 else
 {
                    WORD32 ctb_indx = 0;

 while(ctb_indx != ps_sps->i4_pic_size_in_ctb)
 {
                        WORD32 parse_status = *(ps_codec->pu1_parse_map + ctb_indx);
 volatile WORD32 proc_status = *(ps_codec->pu1_proc_map + ctb_indx) & 1;

 if(parse_status == proc_status)
                            ctb_indx++;
 }
                    ps_codec->s_parse.i4_cur_slice_idx = parse_slice_idx;
 break;
 }

 }
 }

 }
 else
 {
#if FRAME_ILF_PAD
 if(FRAME_ILF_PAD && 1 == ps_codec->i4_num_cores)
 {
 if(ps_slice_hdr->i4_abs_pic_order_cnt == 0)
 {
                DUMP_PRE_ILF(ps_codec->as_process[0].pu1_cur_pic_luma,
                             ps_codec->as_process[0].pu1_cur_pic_chroma,
                             ps_sps->i2_pic_width_in_luma_samples,
                             ps_sps->i2_pic_height_in_luma_samples,
                             ps_codec->i4_strd);

                DUMP_BS(ps_codec->as_process[0].s_bs_ctxt.pu4_pic_vert_bs,
                        ps_codec->as_process[0].s_bs_ctxt.pu4_pic_horz_bs,
                        ps_sps->i2_pic_wd_in_ctb * (ctb_size * ctb_size / 8 / 16) * ps_sps->i2_pic_ht_in_ctb,
 (ps_sps->i2_pic_wd_in_ctb + 1) * (ctb_size * ctb_size / 8 / 16) * ps_sps->i2_pic_ht_in_ctb);

                DUMP_QP(ps_codec->as_process[0].s_bs_ctxt.pu1_pic_qp,
 (ps_sps->i2_pic_height_in_luma_samples * ps_sps->i2_pic_width_in_luma_samples) / (MIN_CU_SIZE * MIN_CU_SIZE));

                DUMP_QP_CONST_IN_CTB(ps_codec->as_process[0].s_bs_ctxt.pu1_pic_qp_const_in_ctb,
 (ps_sps->i2_pic_height_in_luma_samples * ps_sps->i2_pic_width_in_luma_samples) / (MIN_CTB_SIZE * MIN_CTB_SIZE) / 8);

                DUMP_NO_LOOP_FILTER(ps_codec->as_process[0].pu1_pic_no_loop_filter_flag,
 (ps_sps->i2_pic_width_in_luma_samples / MIN_CU_SIZE) * (ps_sps->i2_pic_height_in_luma_samples / MIN_CU_SIZE) / 8);

                DUMP_OFFSETS(ps_slice_hdr->i1_beta_offset_div2,
                             ps_slice_hdr->i1_tc_offset_div2,
                             ps_pps->i1_pic_cb_qp_offset,
                             ps_pps->i1_pic_cr_qp_offset);
 }
            ps_codec->s_parse.s_deblk_ctxt.ps_pps = ps_codec->s_parse.ps_pps;
            ps_codec->s_parse.s_deblk_ctxt.ps_sps = ps_codec->s_parse.ps_sps;
            ps_codec->s_parse.s_deblk_ctxt.ps_codec = ps_codec;
            ps_codec->s_parse.s_deblk_ctxt.ps_slice_hdr = ps_codec->s_parse.ps_slice_hdr;
            ps_codec->s_parse.s_deblk_ctxt.is_chroma_yuv420sp_vu = (ps_codec->e_ref_chroma_fmt == IV_YUV_420SP_VU);

            ps_codec->s_parse.s_sao_ctxt.ps_pps = ps_codec->s_parse.ps_pps;
            ps_codec->s_parse.s_sao_ctxt.ps_sps = ps_codec->s_parse.ps_sps;
            ps_codec->s_parse.s_sao_ctxt.ps_codec = ps_codec;
            ps_codec->s_parse.s_sao_ctxt.ps_slice_hdr = ps_codec->s_parse.ps_slice_hdr;

            ihevcd_ilf_pad_frame(&ps_codec->s_parse.s_deblk_ctxt, &ps_codec->s_parse.s_sao_ctxt);

 }
#endif
        ps_codec->s_parse.i4_end_of_frame = 1;
 }
 return ret;
}
