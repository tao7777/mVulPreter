  unsigned int SAD(unsigned int max_sad, int block_idx = 0) {
    unsigned int ret;
    const uint8_t* const reference = GetReference(block_idx);
    REGISTER_STATE_CHECK(ret = GET_PARAM(2)(source_data_, source_stride_,
                                            reference, reference_stride_,
                                            max_sad));
    return ret;
  }
