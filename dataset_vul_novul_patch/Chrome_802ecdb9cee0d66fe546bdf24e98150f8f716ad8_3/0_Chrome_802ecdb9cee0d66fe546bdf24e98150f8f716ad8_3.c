  void DoCheckFakeData(uint8* audio_data, size_t length) {
    if (algorithm_.is_muted())
      ASSERT_EQ(sum, 0);
    else
      ASSERT_NE(sum, 0);
   }
