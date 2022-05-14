static void setup_token_decoder(VP8D_COMP *pbi,
 const unsigned char* token_part_sizes)
{
    vp8_reader *bool_decoder = &pbi->mbc[0];
 unsigned int partition_idx;
 unsigned int fragment_idx;
 unsigned int num_token_partitions;
 const unsigned char *first_fragment_end = pbi->fragments.ptrs[0] +
                                          pbi->fragments.sizes[0];

    TOKEN_PARTITION multi_token_partition =
 (TOKEN_PARTITION)vp8_read_literal(&pbi->mbc[8], 2);
 if (!vp8dx_bool_error(&pbi->mbc[8]))
        pbi->common.multi_token_partition = multi_token_partition;
    num_token_partitions = 1 << pbi->common.multi_token_partition;

 /* Check for partitions within the fragments and unpack the fragments
     * so that each fragment pointer points to its corresponding partition. */
 for (fragment_idx = 0; fragment_idx < pbi->fragments.count; ++fragment_idx)
 {
 unsigned int fragment_size = pbi->fragments.sizes[fragment_idx];
 const unsigned char *fragment_end = pbi->fragments.ptrs[fragment_idx] +
                                            fragment_size;
 /* Special case for handling the first partition since we have already
         * read its size. */
 if (fragment_idx == 0)
 {
 /* Size of first partition + token partition sizes element */
 ptrdiff_t ext_first_part_size = token_part_sizes -
                pbi->fragments.ptrs[0] + 3 * (num_token_partitions - 1);
            fragment_size -= (unsigned int)ext_first_part_size;
 if (fragment_size > 0)
 {
                pbi->fragments.sizes[0] = (unsigned int)ext_first_part_size;
 /* The fragment contains an additional partition. Move to
                 * next. */
                fragment_idx++;
                pbi->fragments.ptrs[fragment_idx] = pbi->fragments.ptrs[0] +
                  pbi->fragments.sizes[0];
 }
 }
 /* Split the chunk into partitions read from the bitstream */
 while (fragment_size > 0)
 {
 ptrdiff_t partition_size = read_available_partition_size(
                                                 pbi,
                                                 token_part_sizes,
                                                 pbi->fragments.ptrs[fragment_idx],
                                                 first_fragment_end,
                                                 fragment_end,
                                                 fragment_idx - 1,
                                                 num_token_partitions);
            pbi->fragments.sizes[fragment_idx] = (unsigned int)partition_size;
            fragment_size -= (unsigned int)partition_size;
            assert(fragment_idx <= num_token_partitions);
 if (fragment_size > 0)
 {
 /* The fragment contains an additional partition.
                 * Move to next. */
                fragment_idx++;
                pbi->fragments.ptrs[fragment_idx] =
                    pbi->fragments.ptrs[fragment_idx - 1] + partition_size;
 }
 }
 }

    pbi->fragments.count = num_token_partitions + 1;

 for (partition_idx = 1; partition_idx < pbi->fragments.count; ++partition_idx)
 {
 if (vp8dx_start_decode(bool_decoder,
                               pbi->fragments.ptrs[partition_idx],
                               pbi->fragments.sizes[partition_idx],
                               pbi->decrypt_cb, pbi->decrypt_state))
            vpx_internal_error(&pbi->common.error, VPX_CODEC_MEM_ERROR,
 "Failed to allocate bool decoder %d",
                               partition_idx);

        bool_decoder++;

     }
 
 #if CONFIG_MULTITHREAD
  /* Clamp number of decoder threads */
  if (pbi->decoding_thread_count > num_token_partitions - 1) {
    pbi->decoding_thread_count = num_token_partitions - 1;
  }
  if (pbi->decoding_thread_count > pbi->common.mb_rows - 1) {
    pbi->decoding_thread_count = pbi->common.mb_rows - 1;
  }
 #endif
 }
