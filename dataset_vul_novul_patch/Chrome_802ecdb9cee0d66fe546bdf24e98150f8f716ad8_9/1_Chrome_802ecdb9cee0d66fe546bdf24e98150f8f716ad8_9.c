  void WriteFakeData(uint8* audio_data, size_t length) {
    Type* output = reinterpret_cast<Type*>(audio_data);
    for (size_t i = 0; i < length; i++) {
      output[i] = i % 5 + 10;
    }
  }
