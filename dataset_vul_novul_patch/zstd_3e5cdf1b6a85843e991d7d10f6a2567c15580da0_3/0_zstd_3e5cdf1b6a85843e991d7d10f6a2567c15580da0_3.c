static int fuzzerTests(U32 seed, U32 nbTests, unsigned startTest, U32 const maxDurationS, double compressibility, int bigTests)
{
    static const U32 maxSrcLog = 23;
    static const U32 maxSampleLog = 22;
    size_t const srcBufferSize = (size_t)1<<maxSrcLog;
    size_t const dstBufferSize = (size_t)1<<maxSampleLog;
    size_t const cBufferSize   = ZSTD_compressBound(dstBufferSize);
    BYTE* cNoiseBuffer[5];
    BYTE* const cBuffer = (BYTE*) malloc (cBufferSize);
    BYTE* const dstBuffer = (BYTE*) malloc (dstBufferSize);
    BYTE* const mirrorBuffer = (BYTE*) malloc (dstBufferSize);
    ZSTD_CCtx* const refCtx = ZSTD_createCCtx();
    ZSTD_CCtx* const ctx = ZSTD_createCCtx();
    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    U32 result = 0;
    U32 testNb = 0;
    U32 coreSeed = seed;
    UTIL_time_t const startClock = UTIL_getTime();
    U64 const maxClockSpan = maxDurationS * SEC_TO_MICRO;
    int const cLevelLimiter = bigTests ? 3 : 2;

    /* allocation */
    cNoiseBuffer[0] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[1] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[2] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[3] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[4] = (BYTE*)malloc (srcBufferSize);
    CHECK (!cNoiseBuffer[0] || !cNoiseBuffer[1] || !cNoiseBuffer[2] || !cNoiseBuffer[3] || !cNoiseBuffer[4]
           || !dstBuffer || !mirrorBuffer || !cBuffer || !refCtx || !ctx || !dctx,
           "Not enough memory, fuzzer tests cancelled");

    /* Create initial samples */
    RDG_genBuffer(cNoiseBuffer[0], srcBufferSize, 0.00, 0., coreSeed);    /* pure noise */
    RDG_genBuffer(cNoiseBuffer[1], srcBufferSize, 0.05, 0., coreSeed);    /* barely compressible */
    RDG_genBuffer(cNoiseBuffer[2], srcBufferSize, compressibility, 0., coreSeed);
    RDG_genBuffer(cNoiseBuffer[3], srcBufferSize, 0.95, 0., coreSeed);    /* highly compressible */
    RDG_genBuffer(cNoiseBuffer[4], srcBufferSize, 1.00, 0., coreSeed);    /* sparse content */

    /* catch up testNb */
    for (testNb=1; testNb < startTest; testNb++) FUZ_rand(&coreSeed);

    /* main test loop */
    for ( ; (testNb <= nbTests) || (UTIL_clockSpanMicro(startClock) < maxClockSpan); testNb++ ) {
        BYTE* srcBuffer;   /* jumping pointer */
        U32 lseed;
        size_t sampleSize, maxTestSize, totalTestSize;
        size_t cSize, totalCSize, totalGenSize;
        U64 crcOrig;
        BYTE* sampleBuffer;
        const BYTE* dict;
        size_t dictSize;

        /* notification */
        if (nbTests >= testNb) { DISPLAYUPDATE(2, "\r%6u/%6u    ", testNb, nbTests); }
        else { DISPLAYUPDATE(2, "\r%6u          ", testNb); }

        FUZ_rand(&coreSeed);
        { U32 const prime1 = 2654435761U; lseed = coreSeed ^ prime1; }

        /* srcBuffer selection [0-4] */
        {   U32 buffNb = FUZ_rand(&lseed) & 0x7F;
            if (buffNb & 7) buffNb=2;   /* most common : compressible (P) */
            else {
                buffNb >>= 3;
                if (buffNb & 7) {
                    const U32 tnb[2] = { 1, 3 };   /* barely/highly compressible */
                    buffNb = tnb[buffNb >> 3];
                } else {
                    const U32 tnb[2] = { 0, 4 };   /* not compressible / sparse */
                    buffNb = tnb[buffNb >> 3];
            }   }
            srcBuffer = cNoiseBuffer[buffNb];
        }

        /* select src segment */
        sampleSize = FUZ_randomLength(&lseed, maxSampleLog);

        /* create sample buffer (to catch read error with valgrind & sanitizers)  */
        sampleBuffer = (BYTE*)malloc(sampleSize);
        CHECK(sampleBuffer==NULL, "not enough memory for sample buffer");
        { size_t const sampleStart = FUZ_rand(&lseed) % (srcBufferSize - sampleSize);
          memcpy(sampleBuffer, srcBuffer + sampleStart, sampleSize); }
        crcOrig = XXH64(sampleBuffer, sampleSize, 0);

        /* compression tests */
        {   int const cLevelPositive =
                    ( FUZ_rand(&lseed) %
                     (ZSTD_maxCLevel() - (FUZ_highbit32((U32)sampleSize) / cLevelLimiter)) )
                    + 1;
            int const cLevel = ((FUZ_rand(&lseed) & 15) == 3) ?
                             - (int)((FUZ_rand(&lseed) & 7) + 1) :   /* test negative cLevel */
                             cLevelPositive;
            DISPLAYLEVEL(5, "fuzzer t%u: Simple compression test (level %i) \n", testNb, cLevel);
            cSize = ZSTD_compressCCtx(ctx, cBuffer, cBufferSize, sampleBuffer, sampleSize, cLevel);
             CHECK(ZSTD_isError(cSize), "ZSTD_compressCCtx failed : %s", ZSTD_getErrorName(cSize));
 
             /* compression failure test : too small dest buffer */
            assert(cSize > 3);
            {   const size_t missing = (FUZ_rand(&lseed) % (cSize-2)) + 1;
                 const size_t tooSmallSize = cSize - missing;
                 const U32 endMark = 0x4DC2B1A9;
                 memcpy(dstBuffer+tooSmallSize, &endMark, 4);
                DISPLAYLEVEL(5, "fuzzer t%u: compress into too small buffer of size %u (missing %u bytes) \n",
                            testNb, (unsigned)tooSmallSize, (unsigned)missing);
                 { size_t const errorCode = ZSTD_compressCCtx(ctx, dstBuffer, tooSmallSize, sampleBuffer, sampleSize, cLevel);
                   CHECK(!ZSTD_isError(errorCode), "ZSTD_compressCCtx should have failed ! (buffer too small : %u < %u)", (U32)tooSmallSize, (U32)cSize); }
                 { U32 endCheck; memcpy(&endCheck, dstBuffer+tooSmallSize, 4);
                  CHECK(endCheck != endMark, "ZSTD_compressCCtx : dst buffer overflow  (check.%08X != %08X.mark)", endCheck, endMark); }
         }   }
 
         /* frame header decompression test */
        {   ZSTD_frameHeader zfh;
            CHECK_Z( ZSTD_getFrameHeader(&zfh, cBuffer, cSize) );
            CHECK(zfh.frameContentSize != sampleSize, "Frame content size incorrect");
        }

        /* Decompressed size test */
        {   unsigned long long const rSize = ZSTD_findDecompressedSize(cBuffer, cSize);
            CHECK(rSize != sampleSize, "decompressed size incorrect");
        }

        /* successful decompression test */
        DISPLAYLEVEL(5, "fuzzer t%u: simple decompression test \n", testNb);
        {   size_t const margin = (FUZ_rand(&lseed) & 1) ? 0 : (FUZ_rand(&lseed) & 31) + 1;
            size_t const dSize = ZSTD_decompress(dstBuffer, sampleSize + margin, cBuffer, cSize);
            CHECK(dSize != sampleSize, "ZSTD_decompress failed (%s) (srcSize : %u ; cSize : %u)", ZSTD_getErrorName(dSize), (U32)sampleSize, (U32)cSize);
            {   U64 const crcDest = XXH64(dstBuffer, sampleSize, 0);
                CHECK(crcOrig != crcDest, "decompression result corrupted (pos %u / %u)", (U32)findDiff(sampleBuffer, dstBuffer, sampleSize), (U32)sampleSize);
        }   }

        free(sampleBuffer);   /* no longer useful after this point */

        /* truncated src decompression test */
        DISPLAYLEVEL(5, "fuzzer t%u: decompression of truncated source \n", testNb);
        {   size_t const missing = (FUZ_rand(&lseed) % (cSize-2)) + 1;   /* no problem, as cSize > 4 (frameHeaderSizer) */
            size_t const tooSmallSize = cSize - missing;
            void* cBufferTooSmall = malloc(tooSmallSize);   /* valgrind will catch read overflows */
            CHECK(cBufferTooSmall == NULL, "not enough memory !");
            memcpy(cBufferTooSmall, cBuffer, tooSmallSize);
            { size_t const errorCode = ZSTD_decompress(dstBuffer, dstBufferSize, cBufferTooSmall, tooSmallSize);
              CHECK(!ZSTD_isError(errorCode), "ZSTD_decompress should have failed ! (truncated src buffer)"); }
            free(cBufferTooSmall);
        }

        /* too small dst decompression test */
        DISPLAYLEVEL(5, "fuzzer t%u: decompress into too small dst buffer \n", testNb);
        if (sampleSize > 3) {
            size_t const missing = (FUZ_rand(&lseed) % (sampleSize-2)) + 1;   /* no problem, as cSize > 4 (frameHeaderSizer) */
            size_t const tooSmallSize = sampleSize - missing;
            static const BYTE token = 0xA9;
            dstBuffer[tooSmallSize] = token;
            { size_t const errorCode = ZSTD_decompress(dstBuffer, tooSmallSize, cBuffer, cSize);
              CHECK(!ZSTD_isError(errorCode), "ZSTD_decompress should have failed : %u > %u (dst buffer too small)", (U32)errorCode, (U32)tooSmallSize); }
            CHECK(dstBuffer[tooSmallSize] != token, "ZSTD_decompress : dst buffer overflow");
        }

        /* noisy src decompression test */
        if (cSize > 6) {
            /* insert noise into src */
            {   U32 const maxNbBits = FUZ_highbit32((U32)(cSize-4));
                size_t pos = 4;   /* preserve magic number (too easy to detect) */
                for (;;) {
                    /* keep some original src */
                    {   U32 const nbBits = FUZ_rand(&lseed) % maxNbBits;
                        size_t const mask = (1<<nbBits) - 1;
                        size_t const skipLength = FUZ_rand(&lseed) & mask;
                        pos += skipLength;
                    }
                    if (pos >= cSize) break;
                    /* add noise */
                    {   U32 const nbBitsCodes = FUZ_rand(&lseed) % maxNbBits;
                        U32 const nbBits = nbBitsCodes ? nbBitsCodes-1 : 0;
                        size_t const mask = (1<<nbBits) - 1;
                        size_t const rNoiseLength = (FUZ_rand(&lseed) & mask) + 1;
                        size_t const noiseLength = MIN(rNoiseLength, cSize-pos);
                        size_t const noiseStart = FUZ_rand(&lseed) % (srcBufferSize - noiseLength);
                        memcpy(cBuffer + pos, srcBuffer + noiseStart, noiseLength);
                        pos += noiseLength;
            }   }   }

            /* decompress noisy source */
            DISPLAYLEVEL(5, "fuzzer t%u: decompress noisy source \n", testNb);
            {   U32 const endMark = 0xA9B1C3D6;
                memcpy(dstBuffer+sampleSize, &endMark, 4);
                {   size_t const decompressResult = ZSTD_decompress(dstBuffer, sampleSize, cBuffer, cSize);
                    /* result *may* be an unlikely success, but even then, it must strictly respect dst buffer boundaries */
                    CHECK((!ZSTD_isError(decompressResult)) && (decompressResult>sampleSize),
                          "ZSTD_decompress on noisy src : result is too large : %u > %u (dst buffer)", (U32)decompressResult, (U32)sampleSize);
                }
                {   U32 endCheck; memcpy(&endCheck, dstBuffer+sampleSize, 4);
                    CHECK(endMark!=endCheck, "ZSTD_decompress on noisy src : dst buffer overflow");
        }   }   }   /* noisy src decompression test */

        /*=====   Bufferless streaming compression test, scattered segments and dictionary   =====*/
        DISPLAYLEVEL(5, "fuzzer t%u: Bufferless streaming compression test \n", testNb);
        {   U32 const testLog = FUZ_rand(&lseed) % maxSrcLog;
            U32 const dictLog = FUZ_rand(&lseed) % maxSrcLog;
            int const cLevel = (FUZ_rand(&lseed) %
                                (ZSTD_maxCLevel() -
                                 (MAX(testLog, dictLog) / cLevelLimiter))) +
                               1;
            maxTestSize = FUZ_rLogLength(&lseed, testLog);
            if (maxTestSize >= dstBufferSize) maxTestSize = dstBufferSize-1;

            dictSize = FUZ_rLogLength(&lseed, dictLog);   /* needed also for decompression */
            dict = srcBuffer + (FUZ_rand(&lseed) % (srcBufferSize - dictSize));

            DISPLAYLEVEL(6, "fuzzer t%u: Compressing up to <=%u bytes at level %i with dictionary size %u \n",
                            testNb, (U32)maxTestSize, cLevel, (U32)dictSize);

            if (FUZ_rand(&lseed) & 0xF) {
                CHECK_Z ( ZSTD_compressBegin_usingDict(refCtx, dict, dictSize, cLevel) );
            } else {
                ZSTD_compressionParameters const cPar = ZSTD_getCParams(cLevel, ZSTD_CONTENTSIZE_UNKNOWN, dictSize);
                ZSTD_frameParameters const fPar = { FUZ_rand(&lseed)&1 /* contentSizeFlag */,
                                                    !(FUZ_rand(&lseed)&3) /* contentChecksumFlag*/,
                                                    0 /*NodictID*/ };   /* note : since dictionary is fake, dictIDflag has no impact */
                ZSTD_parameters const p = FUZ_makeParams(cPar, fPar);
                CHECK_Z ( ZSTD_compressBegin_advanced(refCtx, dict, dictSize, p, 0) );
            }
            CHECK_Z( ZSTD_copyCCtx(ctx, refCtx, 0) );
        }

        {   U32 const nbChunks = (FUZ_rand(&lseed) & 127) + 2;
            U32 n;
            XXH64_state_t xxhState;
            XXH64_reset(&xxhState, 0);
            for (totalTestSize=0, cSize=0, n=0 ; n<nbChunks ; n++) {
                size_t const segmentSize = FUZ_randomLength(&lseed, maxSampleLog);
                size_t const segmentStart = FUZ_rand(&lseed) % (srcBufferSize - segmentSize);

                if (cBufferSize-cSize < ZSTD_compressBound(segmentSize)) break;   /* avoid invalid dstBufferTooSmall */
                if (totalTestSize+segmentSize > maxTestSize) break;

                {   size_t const compressResult = ZSTD_compressContinue(ctx, cBuffer+cSize, cBufferSize-cSize, srcBuffer+segmentStart, segmentSize);
                    CHECK (ZSTD_isError(compressResult), "multi-segments compression error : %s", ZSTD_getErrorName(compressResult));
                    cSize += compressResult;
                }
                XXH64_update(&xxhState, srcBuffer+segmentStart, segmentSize);
                memcpy(mirrorBuffer + totalTestSize, srcBuffer+segmentStart, segmentSize);
                totalTestSize += segmentSize;
            }

            {   size_t const flushResult = ZSTD_compressEnd(ctx, cBuffer+cSize, cBufferSize-cSize, NULL, 0);
                CHECK (ZSTD_isError(flushResult), "multi-segments epilogue error : %s", ZSTD_getErrorName(flushResult));
                cSize += flushResult;
            }
            crcOrig = XXH64_digest(&xxhState);
        }

        /* streaming decompression test */
        DISPLAYLEVEL(5, "fuzzer t%u: Bufferless streaming decompression test \n", testNb);
        /* ensure memory requirement is good enough (should always be true) */
        {   ZSTD_frameHeader zfh;
            CHECK( ZSTD_getFrameHeader(&zfh, cBuffer, ZSTD_frameHeaderSize_max),
                  "ZSTD_getFrameHeader(): error retrieving frame information");
            {   size_t const roundBuffSize = ZSTD_decodingBufferSize_min(zfh.windowSize, zfh.frameContentSize);
                CHECK_Z(roundBuffSize);
                CHECK((roundBuffSize > totalTestSize) && (zfh.frameContentSize!=ZSTD_CONTENTSIZE_UNKNOWN),
                      "ZSTD_decodingBufferSize_min() requires more memory (%u) than necessary (%u)",
                      (U32)roundBuffSize, (U32)totalTestSize );
        }   }
        if (dictSize<8) dictSize=0, dict=NULL;   /* disable dictionary */
        CHECK_Z( ZSTD_decompressBegin_usingDict(dctx, dict, dictSize) );
        totalCSize = 0;
        totalGenSize = 0;
        while (totalCSize < cSize) {
            size_t const inSize = ZSTD_nextSrcSizeToDecompress(dctx);
            size_t const genSize = ZSTD_decompressContinue(dctx, dstBuffer+totalGenSize, dstBufferSize-totalGenSize, cBuffer+totalCSize, inSize);
            CHECK (ZSTD_isError(genSize), "ZSTD_decompressContinue error : %s", ZSTD_getErrorName(genSize));
            totalGenSize += genSize;
            totalCSize += inSize;
        }
        CHECK (ZSTD_nextSrcSizeToDecompress(dctx) != 0, "frame not fully decoded");
        CHECK (totalGenSize != totalTestSize, "streaming decompressed data : wrong size")
        CHECK (totalCSize != cSize, "compressed data should be fully read")
        {   U64 const crcDest = XXH64(dstBuffer, totalTestSize, 0);
            CHECK(crcOrig != crcDest, "streaming decompressed data corrupted (pos %u / %u)",
                (U32)findDiff(mirrorBuffer, dstBuffer, totalTestSize), (U32)totalTestSize);
        }
    }   /* for ( ; (testNb <= nbTests) */
    DISPLAY("\r%u fuzzer tests completed   \n", testNb-1);

_cleanup:
    ZSTD_freeCCtx(refCtx);
    ZSTD_freeCCtx(ctx);
    ZSTD_freeDCtx(dctx);
    free(cNoiseBuffer[0]);
    free(cNoiseBuffer[1]);
    free(cNoiseBuffer[2]);
    free(cNoiseBuffer[3]);
    free(cNoiseBuffer[4]);
    free(cBuffer);
    free(dstBuffer);
    free(mirrorBuffer);
    return result;

_output_error:
    result = 1;
    goto _cleanup;
}
