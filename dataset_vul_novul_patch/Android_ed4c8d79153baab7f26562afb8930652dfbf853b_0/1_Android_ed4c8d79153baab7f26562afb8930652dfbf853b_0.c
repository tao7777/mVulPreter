 static uint32_t readU16(const uint8_t* data, size_t offset) {
    return data[offset] << 8 | data[offset + 1];
 }
