int vp8_remove_decoder_instances(struct frame_buffers *fb)
{
 if(!fb->use_frame_threads)
 {
        VP8D_COMP *pbi = fb->pbi[0];


         if (!pbi)
             return VPX_CODEC_ERROR;
 #if CONFIG_MULTITHREAD
         vp8_decoder_remove_threads(pbi);
 #endif
 
 /* decoder instance for single thread mode */
        remove_decompressor(pbi);
 }
 else
 {
 /* TODO : remove frame threads and decoder instances for each
         * thread here */
 }

 return VPX_CODEC_OK;
}
