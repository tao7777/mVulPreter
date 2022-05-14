  void CheckFakeData(uint8* audio_data, int frames_written,
                     double playback_rate) {
    size_t length =
        (frames_written * algorithm_.bytes_per_frame())
        / algorithm_.bytes_per_channel();
    switch (algorithm_.bytes_per_channel()) {
      case 4:
        DoCheckFakeData<int32>(audio_data, length);
        break;
      case 2:
        DoCheckFakeData<int16>(audio_data, length);
        break;
      case 1:
        DoCheckFakeData<uint8>(audio_data, length);
        break;
      default:
        NOTREACHED() << "Unsupported audio bit depth in crossfade.";
    }
  }
