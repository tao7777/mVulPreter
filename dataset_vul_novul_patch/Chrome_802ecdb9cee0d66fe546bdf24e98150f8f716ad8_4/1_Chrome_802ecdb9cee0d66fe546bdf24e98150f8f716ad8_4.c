  void EnqueueData() {
     scoped_array<uint8> audio_data(new uint8[kRawDataSize]);
     CHECK_EQ(kRawDataSize % algorithm_.bytes_per_channel(), 0u);
     CHECK_EQ(kRawDataSize % algorithm_.bytes_per_frame(), 0u);
    size_t length = kRawDataSize / algorithm_.bytes_per_channel();
    switch (algorithm_.bytes_per_channel()) {
      case 4:
        WriteFakeData<int32>(audio_data.get(), length);
        break;
      case 2:
        WriteFakeData<int16>(audio_data.get(), length);
        break;
      case 1:
        WriteFakeData<uint8>(audio_data.get(), length);
        break;
      default:
        NOTREACHED() << "Unsupported audio bit depth in crossfade.";
    }
     algorithm_.EnqueueBuffer(new DataBuffer(audio_data.Pass(), kRawDataSize));
     bytes_enqueued_ += kRawDataSize;
   }
