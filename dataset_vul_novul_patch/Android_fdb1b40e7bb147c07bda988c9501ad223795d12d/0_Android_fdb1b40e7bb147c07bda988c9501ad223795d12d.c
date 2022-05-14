int vp9_alloc_context_buffers(VP9_COMMON *cm, int width, int height) {
 int new_mi_size;

  vp9_set_mb_mi(cm, width, height);
  new_mi_size = cm->mi_stride * calc_mi_size(cm->mi_rows);
 if (cm->mi_alloc_size < new_mi_size) {
    cm->free_mi(cm);
 if (cm->alloc_mi(cm, new_mi_size))
 goto fail;
 }

 if (cm->seg_map_alloc_size < cm->mi_rows * cm->mi_cols) {
    free_seg_map(cm);
 if (alloc_seg_map(cm, cm->mi_rows * cm->mi_cols))
 goto fail;
 }

 if (cm->above_context_alloc_cols < cm->mi_cols) {
    vpx_free(cm->above_context);
    cm->above_context = (ENTROPY_CONTEXT *)vpx_calloc(
 2 * mi_cols_aligned_to_sb(cm->mi_cols) * MAX_MB_PLANE,
 sizeof(*cm->above_context));
 if (!cm->above_context) goto fail;

    vpx_free(cm->above_seg_context);
    cm->above_seg_context = (PARTITION_CONTEXT *)vpx_calloc(
        mi_cols_aligned_to_sb(cm->mi_cols), sizeof(*cm->above_seg_context));
 if (!cm->above_seg_context) goto fail;
    cm->above_context_alloc_cols = cm->mi_cols;
 }


   return 0;
 
  fail:
  vp9_set_mb_mi(cm, 0, 0);
   vp9_free_context_buffers(cm);
   return 1;
 }
