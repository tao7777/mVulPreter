ZSTD_buildCTable(void* dst, size_t dstCapacity,
                FSE_CTable* nextCTable, U32 FSELog, symbolEncodingType_e type,
                U32* count, U32 max,
                const BYTE* codeTable, size_t nbSeq,
                const S16* defaultNorm, U32 defaultNormLog, U32 defaultMax,
                const FSE_CTable* prevCTable, size_t prevCTableSize,
                void* workspace, size_t workspaceSize)
 {
     BYTE* op = (BYTE*)dst;
     const BYTE* const oend = op + dstCapacity;
 
     switch (type) {
     case set_rle:
        *op = codeTable[0];
         CHECK_F(FSE_buildCTable_rle(nextCTable, (BYTE)max));
         return 1;
     case set_repeat:
         memcpy(nextCTable, prevCTable, prevCTableSize);
        return 0;
    case set_basic:
        CHECK_F(FSE_buildCTable_wksp(nextCTable, defaultNorm, defaultMax, defaultNormLog, workspace, workspaceSize));  /* note : could be pre-calculated */
        return 0;
    case set_compressed: {
        S16 norm[MaxSeq + 1];
        size_t nbSeq_1 = nbSeq;
        const U32 tableLog = FSE_optimalTableLog(FSELog, nbSeq, max);
        if (count[codeTable[nbSeq-1]] > 1) {
            count[codeTable[nbSeq-1]]--;
            nbSeq_1--;
        }
        assert(nbSeq_1 > 1);
        CHECK_F(FSE_normalizeCount(norm, tableLog, count, nbSeq_1, max));
        {   size_t const NCountSize = FSE_writeNCount(op, oend - op, norm, max, tableLog);   /* overflow protected */
            if (FSE_isError(NCountSize)) return NCountSize;
            CHECK_F(FSE_buildCTable_wksp(nextCTable, norm, max, tableLog, workspace, workspaceSize));
            return NCountSize;
        }
    }
    default: return assert(0), ERROR(GENERIC);
    }
}
