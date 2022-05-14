 void TestBlinkPlatformSupport::cryptographicallyRandomValues(
     unsigned char* buffer,
     size_t length) {
  base::RandBytes(buffer, length);
 }
