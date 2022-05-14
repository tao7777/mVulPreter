static size_t ZSTD_encodeSequences(
            void* dst, size_t dstCapacity,
            FSE_CTable const* CTable_MatchLength, BYTE const* mlCodeTable,
            FSE_CTable const* CTable_OffsetBits, BYTE const* ofCodeTable,
             FSE_CTable const* CTable_LitLength, BYTE const* llCodeTable,
             seqDef const* sequences, size_t nbSeq, int longOffsets, int bmi2)
 {
 #if DYNAMIC_BMI2
     if (bmi2) {
         return ZSTD_encodeSequences_bmi2(dst, dstCapacity,
                                         CTable_MatchLength, mlCodeTable,
                                         CTable_OffsetBits, ofCodeTable,
                                         CTable_LitLength, llCodeTable,
                                         sequences, nbSeq, longOffsets);
    }
#endif
    (void)bmi2;
    return ZSTD_encodeSequences_default(dst, dstCapacity,
                                        CTable_MatchLength, mlCodeTable,
                                        CTable_OffsetBits, ofCodeTable,
                                        CTable_LitLength, llCodeTable,
                                        sequences, nbSeq, longOffsets);
}
