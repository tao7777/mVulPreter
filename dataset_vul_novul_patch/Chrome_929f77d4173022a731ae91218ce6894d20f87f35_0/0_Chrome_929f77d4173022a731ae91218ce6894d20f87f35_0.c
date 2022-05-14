static bool CheckDts(const uint8_t* buffer, int buffer_size) {
  RCHECK(buffer_size > 11);

  int offset = 0;
  while (offset + 11 < buffer_size) {
    BitReader reader(buffer + offset, 11);

    RCHECK(ReadBits(&reader, 32) == 0x7ffe8001);

    reader.SkipBits(1 + 5);

    RCHECK(ReadBits(&reader, 1) == 0);  // CPF must be 0.

    RCHECK(ReadBits(&reader, 7) >= 5);

    int frame_size = ReadBits(&reader, 14);
    RCHECK(frame_size >= 95);

     reader.SkipBits(6);
 
    size_t sampling_freq_index = ReadBits(&reader, 4);
    RCHECK(sampling_freq_index < base::size(kSamplingFrequencyValid));
    RCHECK(kSamplingFrequencyValid[sampling_freq_index]);
 
     RCHECK(ReadBits(&reader, 5) <= 25);

    RCHECK(ReadBits(&reader, 1) == 0);

     reader.SkipBits(1 + 1 + 1 + 1);
 
    size_t audio_id_index = ReadBits(&reader, 3);
    RCHECK(audio_id_index < base::size(kExtAudioIdValid));
    RCHECK(kExtAudioIdValid[audio_id_index]);
 
     reader.SkipBits(1 + 1);

    RCHECK(ReadBits(&reader, 2) != 3);

    offset += frame_size + 1;
  }
  return true;
}
