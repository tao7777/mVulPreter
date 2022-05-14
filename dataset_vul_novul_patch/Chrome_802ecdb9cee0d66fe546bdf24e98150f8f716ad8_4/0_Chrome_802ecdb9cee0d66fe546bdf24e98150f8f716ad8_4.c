  void EnqueueData() {
     scoped_array<uint8> audio_data(new uint8[kRawDataSize]);
     CHECK_EQ(kRawDataSize % algorithm_.bytes_per_channel(), 0u);
     CHECK_EQ(kRawDataSize % algorithm_.bytes_per_frame(), 0u);
    // The value of the data is meaningless; we just want non-zero data to
    // differentiate it from muted data.
    memset(audio_data.get(), 1, kRawDataSize);
     algorithm_.EnqueueBuffer(new DataBuffer(audio_data.Pass(), kRawDataSize));
     bytes_enqueued_ += kRawDataSize;
   }
