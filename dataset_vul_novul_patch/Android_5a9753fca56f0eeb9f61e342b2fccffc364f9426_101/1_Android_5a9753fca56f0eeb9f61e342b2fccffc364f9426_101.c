   void SADs(unsigned int *results) {
    const uint8_t* refs[] = {GetReference(0), GetReference(1),
                             GetReference(2), GetReference(3)};
 
    REGISTER_STATE_CHECK(GET_PARAM(2)(source_data_, source_stride_,
                                      refs, reference_stride_,
                                      results));
   }
