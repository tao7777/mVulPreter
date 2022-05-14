   static void TearDownTestCase() {
    vpx_free(source_data_);
    source_data_ = NULL;
    vpx_free(reference_data_);
    reference_data_ = NULL;
   }
