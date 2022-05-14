 static uint32_t readU32(const uint8_t* data, size_t offset) {
    return ((uint32_t)data[offset]) << 24 | ((uint32_t)data[offset + 1]) << 16 |
        ((uint32_t)data[offset + 2]) << 8 | ((uint32_t)data[offset + 3]);
 }
