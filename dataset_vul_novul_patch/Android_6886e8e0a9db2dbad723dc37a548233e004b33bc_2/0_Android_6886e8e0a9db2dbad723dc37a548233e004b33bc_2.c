void vp8_decoder_remove_threads(VP8D_COMP *pbi)
void vp8_decoder_remove_threads(VP8D_COMP *pbi) {
  /* shutdown MB Decoding thread; */
  if (pbi->b_multithreaded_rd) {
    int i;
    pbi->b_multithreaded_rd = 0;
 
    /* allow all threads to exit */
    for (i = 0; i < pbi->allocated_decoding_thread_count; ++i) {
      sem_post(&pbi->h_event_start_decoding[i]);
      pthread_join(pbi->h_decoding_thread[i], NULL);
     }

    for (i = 0; i < pbi->allocated_decoding_thread_count; ++i) {
      sem_destroy(&pbi->h_event_start_decoding[i]);
    }

    sem_destroy(&pbi->h_event_end_decoding);

    vpx_free(pbi->h_decoding_thread);
    pbi->h_decoding_thread = NULL;

    vpx_free(pbi->h_event_start_decoding);
    pbi->h_event_start_decoding = NULL;

    vpx_free(pbi->mb_row_di);
    pbi->mb_row_di = NULL;

    vpx_free(pbi->de_thread_data);
    pbi->de_thread_data = NULL;

    vp8mt_de_alloc_temp_buffers(pbi, pbi->common.mb_rows);
  }
 }
