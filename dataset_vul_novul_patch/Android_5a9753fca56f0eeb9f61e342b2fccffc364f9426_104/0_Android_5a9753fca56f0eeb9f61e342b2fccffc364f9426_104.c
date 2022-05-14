   static void TearDownTestCase() {
    vpx_free(source_data8_);
    source_data8_ = NULL;
    vpx_free(reference_data8_);
    reference_data8_ = NULL;
    vpx_free(second_pred8_);
    second_pred8_ = NULL;
    vpx_free(source_data16_);
    source_data16_ = NULL;
    vpx_free(reference_data16_);
    reference_data16_ = NULL;
    vpx_free(second_pred16_);
    second_pred16_ = NULL;
   }
