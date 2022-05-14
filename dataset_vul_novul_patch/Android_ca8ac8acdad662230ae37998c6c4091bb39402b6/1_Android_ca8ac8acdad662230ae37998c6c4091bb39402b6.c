bool CmapCoverage::getCoverage(SparseBitSet& coverage, const uint8_t* cmap_data, size_t cmap_size) {
 vector<uint32_t> coverageVec;
 const size_t kHeaderSize = 4;
 const size_t kNumTablesOffset = 2;
 const size_t kTableSize = 8;

     const size_t kPlatformIdOffset = 0;
     const size_t kEncodingIdOffset = 2;
     const size_t kOffsetOffset = 4;
    const int kMicrosoftPlatformId = 3;
    const int kUnicodeBmpEncodingId = 1;
    const int kUnicodeUcs4EncodingId = 10;
     if (kHeaderSize > cmap_size) {
         return false;
     }
    int numTables = readU16(cmap_data, kNumTablesOffset);
     if (kHeaderSize + numTables * kTableSize > cmap_size) {
         return false;
     }
    int bestTable = -1;
    for (int i = 0; i < numTables; i++) {
         uint16_t platformId = readU16(cmap_data, kHeaderSize + i * kTableSize + kPlatformIdOffset);
         uint16_t encodingId = readU16(cmap_data, kHeaderSize + i * kTableSize + kEncodingIdOffset);
         if (platformId == kMicrosoftPlatformId && encodingId == kUnicodeUcs4EncodingId) {
            bestTable = i;
 break;
 } else if (platformId == kMicrosoftPlatformId && encodingId == kUnicodeBmpEncodingId) {
            bestTable = i;
 }
 }

 #ifdef PRINTF_DEBUG
     printf("best table = %d\n", bestTable);
 #endif
    if (bestTable < 0) {
         return false;
     }
     uint32_t offset = readU32(cmap_data, kHeaderSize + bestTable * kTableSize + kOffsetOffset);
    if (offset + 2 > cmap_size) {
         return false;
     }
     uint16_t format = readU16(cmap_data, offset);
 bool success = false;
 const uint8_t* tableData = cmap_data + offset;
 const size_t tableSize = cmap_size - offset;
 if (format == 4) {
        success = getCoverageFormat4(coverageVec, tableData, tableSize);
 } else if (format == 12) {
        success = getCoverageFormat12(coverageVec, tableData, tableSize);
 }
 if (success) {
        coverage.initFromRanges(&coverageVec.front(), coverageVec.size() >> 1);
 }
#ifdef PRINTF_DEBUG
 for (int i = 0; i < coverageVec.size(); i += 2) {
        printf("%x:%x\n", coverageVec[i], coverageVec[i + 1]);
 }
#endif
 return success;
}
