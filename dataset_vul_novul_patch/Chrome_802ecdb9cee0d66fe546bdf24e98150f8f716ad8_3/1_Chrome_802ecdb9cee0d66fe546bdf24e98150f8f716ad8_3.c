  void DoCheckFakeData(uint8* audio_data, size_t length) {
    Type* output = reinterpret_cast<Type*>(audio_data);
    for (size_t i = 0; i < length; i++) {
      EXPECT_TRUE(algorithm_.is_muted() || output[i] != 0);
    }
   }
