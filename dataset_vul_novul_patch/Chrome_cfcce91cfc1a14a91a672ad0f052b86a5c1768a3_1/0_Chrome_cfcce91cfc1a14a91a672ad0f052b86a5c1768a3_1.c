bool FoldChannels(void* buf,
                  size_t buflen,
                  int channels,
                  int bytes_per_sample,
                   float volume) {
   DCHECK(buf);
   if (volume < 0.0f || volume > 1.0f)
    return false;
   if (channels > 2 && channels <= 8 && bytes_per_sample > 0) {
     int sample_count = buflen / (channels * bytes_per_sample);
     if (bytes_per_sample == 1) {
      FoldChannels<uint8, int32, -128, 127, 128>(
          reinterpret_cast<uint8*>(buf),
          sample_count,
          volume,
          channels);
      return true;
    } else if (bytes_per_sample == 2) {
      FoldChannels<int16, int32, -32768, 32767, 0>(
          reinterpret_cast<int16*>(buf),
          sample_count,
          volume,
          channels);
      return true;
    } else if (bytes_per_sample == 4) {
      FoldChannels<int32, int64, 0x80000000, 0x7fffffff, 0>(
          reinterpret_cast<int32*>(buf),
          sample_count,
          volume,
          channels);
      return true;
    }
  }
  return false;
}
