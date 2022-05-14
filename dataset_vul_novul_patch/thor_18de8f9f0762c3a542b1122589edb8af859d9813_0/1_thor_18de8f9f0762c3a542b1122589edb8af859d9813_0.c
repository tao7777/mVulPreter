void read_sequence_header(decoder_info_t *decoder_info, stream_t *stream) {
   decoder_info->width = get_flc(16, stream);
   decoder_info->height = get_flc(16, stream);
   decoder_info->log2_sb_size = get_flc(3, stream);
   decoder_info->pb_split = get_flc(1, stream);
   decoder_info->tb_split_enable = get_flc(1, stream);
   decoder_info->max_num_ref = get_flc(2, stream) + 1;
  decoder_info->interp_ref = get_flc(2, stream);
  decoder_info->max_delta_qp = get_flc(1, stream);
  decoder_info->deblocking = get_flc(1, stream);
  decoder_info->clpf = get_flc(1, stream);
  decoder_info->use_block_contexts = get_flc(1, stream);
  decoder_info->bipred = get_flc(2, stream);
  decoder_info->qmtx = get_flc(1, stream);
  if (decoder_info->qmtx) {
    decoder_info->qmtx_offset = get_flc(6, stream) - 32;
  }
  decoder_info->subsample = get_flc(2, stream);
    decoder_info->subsample = // 0: 400  1: 420  2: 422  3: 444
    (decoder_info->subsample & 1) * 20 + (decoder_info->subsample & 2) * 22 +
    ((decoder_info->subsample & 3) == 3) * 2 + 400;
  decoder_info->num_reorder_pics = get_flc(4, stream);
  if (decoder_info->subsample != 400) {
    decoder_info->cfl_intra = get_flc(1, stream);
    decoder_info->cfl_inter = get_flc(1, stream);
  }
  decoder_info->bitdepth = get_flc(1, stream) ? 10 : 8;
  if (decoder_info->bitdepth == 10)
    decoder_info->bitdepth += 2 * get_flc(1, stream);
  decoder_info->input_bitdepth = get_flc(1, stream) ? 10 : 8;
  if (decoder_info->input_bitdepth == 10)
    decoder_info->input_bitdepth += 2 * get_flc(1, stream);
}
