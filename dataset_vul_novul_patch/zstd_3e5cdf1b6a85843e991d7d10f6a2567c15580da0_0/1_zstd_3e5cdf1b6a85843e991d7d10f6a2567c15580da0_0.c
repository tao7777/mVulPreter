ZSTD_compressSequences_internal(seqStore_t* seqStorePtr,
                              ZSTD_entropyCTables_t const* prevEntropy,
                              ZSTD_entropyCTables_t* nextEntropy,
                              ZSTD_CCtx_params const* cctxParams,
                              void* dst, size_t dstCapacity,
                              void* workspace, size_t wkspSize,
                              const int bmi2)
{
    const int longOffsets = cctxParams->cParams.windowLog > STREAM_ACCUMULATOR_MIN;
    ZSTD_strategy const strategy = cctxParams->cParams.strategy;
    U32 count[MaxSeq+1];
    FSE_CTable* CTable_LitLength = nextEntropy->fse.litlengthCTable;
    FSE_CTable* CTable_OffsetBits = nextEntropy->fse.offcodeCTable;
    FSE_CTable* CTable_MatchLength = nextEntropy->fse.matchlengthCTable;
    U32 LLtype, Offtype, MLtype;   /* compressed, raw or rle */
    const seqDef* const sequences = seqStorePtr->sequencesStart;
    const BYTE* const ofCodeTable = seqStorePtr->ofCode;
    const BYTE* const llCodeTable = seqStorePtr->llCode;
    const BYTE* const mlCodeTable = seqStorePtr->mlCode;
    BYTE* const ostart = (BYTE*)dst;
    BYTE* const oend = ostart + dstCapacity;
    BYTE* op = ostart;
    size_t const nbSeq = seqStorePtr->sequences - seqStorePtr->sequencesStart;
    BYTE* seqHead;
    BYTE* lastNCount = NULL;

    ZSTD_STATIC_ASSERT(HUF_WORKSPACE_SIZE >= (1<<MAX(MLFSELog,LLFSELog)));

    /* Compress literals */
    {   const BYTE* const literals = seqStorePtr->litStart;
        size_t const litSize = seqStorePtr->lit - literals;
        int const disableLiteralCompression = (cctxParams->cParams.strategy == ZSTD_fast) && (cctxParams->cParams.targetLength > 0);
        size_t const cSize = ZSTD_compressLiterals(
                                    &prevEntropy->huf, &nextEntropy->huf,
                                    cctxParams->cParams.strategy, disableLiteralCompression,
                                    op, dstCapacity,
                                    literals, litSize,
                                    workspace, wkspSize,
                                    bmi2);
        if (ZSTD_isError(cSize))
          return cSize;
        assert(cSize <= dstCapacity);
        op += cSize;
    }

    /* Sequences Header */
    if ((oend-op) < 3 /*max nbSeq Size*/ + 1 /*seqHead*/) return ERROR(dstSize_tooSmall);
    if (nbSeq < 0x7F)
        *op++ = (BYTE)nbSeq;
    else if (nbSeq < LONGNBSEQ)
        op[0] = (BYTE)((nbSeq>>8) + 0x80), op[1] = (BYTE)nbSeq, op+=2;
    else
        op[0]=0xFF, MEM_writeLE16(op+1, (U16)(nbSeq - LONGNBSEQ)), op+=3;
    if (nbSeq==0) {
        /* Copy the old tables over as if we repeated them */
        memcpy(&nextEntropy->fse, &prevEntropy->fse, sizeof(prevEntropy->fse));
        return op - ostart;
    }

    /* seqHead : flags for FSE encoding type */
    seqHead = op++;

    /* convert length/distances into codes */
    ZSTD_seqToCodes(seqStorePtr);
    /* build CTable for Literal Lengths */
    {   U32 max = MaxLL;
        size_t const mostFrequent = HIST_countFast_wksp(count, &max, llCodeTable, nbSeq, workspace, wkspSize);   /* can't fail */
        DEBUGLOG(5, "Building LL table");
        nextEntropy->fse.litlength_repeatMode = prevEntropy->fse.litlength_repeatMode;
        LLtype = ZSTD_selectEncodingType(&nextEntropy->fse.litlength_repeatMode, count, max, mostFrequent, nbSeq, LLFSELog, prevEntropy->fse.litlengthCTable, LL_defaultNorm, LL_defaultNormLog, ZSTD_defaultAllowed, strategy);
        assert(set_basic < set_compressed && set_rle < set_compressed);
        assert(!(LLtype < set_compressed && nextEntropy->fse.litlength_repeatMode != FSE_repeat_none)); /* We don't copy tables */
        {   size_t const countSize = ZSTD_buildCTable(op, oend - op, CTable_LitLength, LLFSELog, (symbolEncodingType_e)LLtype,
                                                    count, max, llCodeTable, nbSeq, LL_defaultNorm, LL_defaultNormLog, MaxLL,
                                                    prevEntropy->fse.litlengthCTable, sizeof(prevEntropy->fse.litlengthCTable),
                                                    workspace, wkspSize);
            if (ZSTD_isError(countSize)) return countSize;
            if (LLtype == set_compressed)
                lastNCount = op;
            op += countSize;
    }   }
    /* build CTable for Offsets */
    {   U32 max = MaxOff;
        size_t const mostFrequent = HIST_countFast_wksp(count, &max, ofCodeTable, nbSeq, workspace, wkspSize);  /* can't fail */
        /* We can only use the basic table if max <= DefaultMaxOff, otherwise the offsets are too large */
        ZSTD_defaultPolicy_e const defaultPolicy = (max <= DefaultMaxOff) ? ZSTD_defaultAllowed : ZSTD_defaultDisallowed;
        DEBUGLOG(5, "Building OF table");
        nextEntropy->fse.offcode_repeatMode = prevEntropy->fse.offcode_repeatMode;
        Offtype = ZSTD_selectEncodingType(&nextEntropy->fse.offcode_repeatMode, count, max, mostFrequent, nbSeq, OffFSELog, prevEntropy->fse.offcodeCTable, OF_defaultNorm, OF_defaultNormLog, defaultPolicy, strategy);
        assert(!(Offtype < set_compressed && nextEntropy->fse.offcode_repeatMode != FSE_repeat_none)); /* We don't copy tables */
        {   size_t const countSize = ZSTD_buildCTable(op, oend - op, CTable_OffsetBits, OffFSELog, (symbolEncodingType_e)Offtype,
                                                    count, max, ofCodeTable, nbSeq, OF_defaultNorm, OF_defaultNormLog, DefaultMaxOff,
                                                    prevEntropy->fse.offcodeCTable, sizeof(prevEntropy->fse.offcodeCTable),
                                                    workspace, wkspSize);
            if (ZSTD_isError(countSize)) return countSize;
            if (Offtype == set_compressed)
                lastNCount = op;
            op += countSize;
    }   }
     /* build CTable for MatchLengths */
     {   U32 max = MaxML;
         size_t const mostFrequent = HIST_countFast_wksp(count, &max, mlCodeTable, nbSeq, workspace, wkspSize);   /* can't fail */
        DEBUGLOG(5, "Building ML table");
         nextEntropy->fse.matchlength_repeatMode = prevEntropy->fse.matchlength_repeatMode;
         MLtype = ZSTD_selectEncodingType(&nextEntropy->fse.matchlength_repeatMode, count, max, mostFrequent, nbSeq, MLFSELog, prevEntropy->fse.matchlengthCTable, ML_defaultNorm, ML_defaultNormLog, ZSTD_defaultAllowed, strategy);
         assert(!(MLtype < set_compressed && nextEntropy->fse.matchlength_repeatMode != FSE_repeat_none)); /* We don't copy tables */
        {   size_t const countSize = ZSTD_buildCTable(op, oend - op, CTable_MatchLength, MLFSELog, (symbolEncodingType_e)MLtype,
                                                    count, max, mlCodeTable, nbSeq, ML_defaultNorm, ML_defaultNormLog, MaxML,
                                                    prevEntropy->fse.matchlengthCTable, sizeof(prevEntropy->fse.matchlengthCTable),
                                                    workspace, wkspSize);
            if (ZSTD_isError(countSize)) return countSize;
            if (MLtype == set_compressed)
                lastNCount = op;
            op += countSize;
    }   }

    *seqHead = (BYTE)((LLtype<<6) + (Offtype<<4) + (MLtype<<2));

    {   size_t const bitstreamSize = ZSTD_encodeSequences(
                                        op, oend - op,
                                        CTable_MatchLength, mlCodeTable,
                                        CTable_OffsetBits, ofCodeTable,
                                        CTable_LitLength, llCodeTable,
                                        sequences, nbSeq,
                                        longOffsets, bmi2);
        if (ZSTD_isError(bitstreamSize)) return bitstreamSize;
        op += bitstreamSize;
        /* zstd versions <= 1.3.4 mistakenly report corruption when
         * FSE_readNCount() recieves a buffer < 4 bytes.
         * Fixed by https://github.com/facebook/zstd/pull/1146.
         * This can happen when the last set_compressed table present is 2
         * bytes and the bitstream is only one byte.
         * In this exceedingly rare case, we will simply emit an uncompressed
         * block, since it isn't worth optimizing.
         */
        if (lastNCount && (op - lastNCount) < 4) {
            /* NCountSize >= 2 && bitstreamSize > 0 ==> lastCountSize == 3 */
            assert(op - lastNCount == 3);
            DEBUGLOG(5, "Avoiding bug in zstd decoder in versions <= 1.3.4 by "
                        "emitting an uncompressed block.");
            return 0;
        }
    }

    return op - ostart;
}
