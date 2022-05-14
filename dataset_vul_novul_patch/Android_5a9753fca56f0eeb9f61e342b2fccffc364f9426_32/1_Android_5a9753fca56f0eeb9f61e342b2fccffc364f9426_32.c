   static void TearDownTestCase() {
     vpx_free(input_ - 1);
     input_ = NULL;
     vpx_free(output_);
     output_ = NULL;
   }
