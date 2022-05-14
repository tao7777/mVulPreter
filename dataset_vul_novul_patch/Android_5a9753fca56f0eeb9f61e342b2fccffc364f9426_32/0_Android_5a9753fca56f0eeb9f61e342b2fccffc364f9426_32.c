   static void TearDownTestCase() {
     vpx_free(input_ - 1);
     input_ = NULL;
     vpx_free(output_);
     output_ = NULL;
    vpx_free(output_ref_);
    output_ref_ = NULL;
#if CONFIG_VP9_HIGHBITDEPTH
    vpx_free(input16_ - 1);
    input16_ = NULL;
    vpx_free(output16_);
    output16_ = NULL;
    vpx_free(output16_ref_);
    output16_ref_ = NULL;
#endif
   }
