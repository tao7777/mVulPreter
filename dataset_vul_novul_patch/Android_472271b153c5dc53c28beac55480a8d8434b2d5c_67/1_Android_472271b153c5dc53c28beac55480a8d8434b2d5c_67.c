 void *atomic_thread(void *context) {
   struct atomic_test_s32_s *at = (struct atomic_test_s32_s *)context;
   for (int i = 0; i < at->max_val; i++) {
    usleep(1);
     atomic_inc_prefix_s32(&at->data[i]);
   }
   return NULL;
}
