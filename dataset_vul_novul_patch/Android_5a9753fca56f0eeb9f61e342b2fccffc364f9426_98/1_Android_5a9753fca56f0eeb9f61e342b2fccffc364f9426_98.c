  virtual uint8_t* GetReference(int block_idx) {
     return reference_data_ + block_idx * kDataBlockSize;
   }
