 static uint32_t readU16(const uint8_t* data, size_t offset) {
    return ((uint32_t)data[offset]) << 8 | ((uint32_t)data[offset + 1]);
 }
