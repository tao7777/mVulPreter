 static uint32_t readU32(const uint8_t* data, size_t offset) {
    return data[offset] << 24 | data[offset + 1] << 16 | data[offset + 2] << 8 | data[offset + 3];
 }
