static EAS_RESULT Parse_wave (SDLS_SYNTHESIZER_DATA *pDLSData, EAS_I32 pos, EAS_U16 waveIndex)
{
    EAS_RESULT result;
    EAS_U32 temp;
    EAS_I32 size;
    EAS_I32 endChunk;
    EAS_I32 chunkPos;
    EAS_I32 wsmpPos = 0;
    EAS_I32 fmtPos = 0;
    EAS_I32 dataPos = 0;
    EAS_I32 dataSize = 0;
    S_WSMP_DATA *p;
 void *pSample;
    S_WSMP_DATA wsmp;

 /* seek to start of chunk */
    chunkPos = pos + 12;
 if ((result = EAS_HWFileSeek(pDLSData->hwInstData, pDLSData->fileHandle, pos)) != EAS_SUCCESS)
 return result;

 /* get the chunk type */
 if ((result = NextChunk(pDLSData, &pos, &temp, &size)) != EAS_SUCCESS)
 return result;

 /* make sure it is a wave chunk */
 if (temp != CHUNK_WAVE)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "Offset in ptbl does not point to wave chunk\n"); */ }
 return EAS_ERROR_FILE_FORMAT;
 }

 /* read to end of chunk */
    pos = chunkPos;
    endChunk = pos + size;
 while (pos < endChunk)
 {
        chunkPos = pos;

 /* get the chunk type */
 if ((result = NextChunk(pDLSData, &pos, &temp, &size)) != EAS_SUCCESS)
 return result;

 /* parse useful chunks */
 switch (temp)
 {
 case CHUNK_WSMP:
                wsmpPos = chunkPos + 8;
 break;

 case CHUNK_FMT:
                fmtPos = chunkPos + 8;
 break;

 case CHUNK_DATA:
                dataPos = chunkPos + 8;
                dataSize = size;
 break;

 default:
 break;
 }

     }
 
    if (dataSize < 0 || dataSize > MAX_DLS_WAVE_SIZE)
     {
         return EAS_ERROR_SOUND_LIBRARY;
     }

 /* for first pass, use temporary variable */
 if (pDLSData->pDLS == NULL)
        p = &wsmp;
 else
        p = &pDLSData->wsmpData[waveIndex];

 /* set the defaults */
    p->fineTune = 0;
    p->unityNote = 60;
    p->gain = 0;
    p->loopStart = 0;
    p->loopLength = 0;

 /* must have a fmt chunk */
 if (!fmtPos)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "DLS wave chunk has no fmt chunk\n"); */ }
 return EAS_ERROR_UNRECOGNIZED_FORMAT;
 }

 /* must have a data chunk */
 if (!dataPos)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "DLS wave chunk has no data chunk\n"); */ }
 return EAS_ERROR_UNRECOGNIZED_FORMAT;
 }

 /* parse the wsmp chunk */
 if (wsmpPos)
 {
 if ((result = Parse_wsmp(pDLSData, wsmpPos, p)) != EAS_SUCCESS)
 return result;
 }

 /* parse the fmt chunk */
 if ((result = Parse_fmt(pDLSData, fmtPos, p)) != EAS_SUCCESS)
 return result;

 /* calculate the size of the wavetable needed. We need only half
     * the memory for 16-bit samples when in 8-bit mode, and we need
     * double the memory for 8-bit samples in 16-bit mode. For
     * unlooped samples, we may use ADPCM. If so, we need only 1/4
     * the memory.
     *
     * We also need to add one for looped samples to allow for
     * the first sample to be copied to the end of the loop.
     */

 /* use ADPCM encode for unlooped 16-bit samples if ADPCM is enabled */
 /*lint -e{506} -e{774} groundwork for future version to support 8 & 16 bit */
 if (bitDepth == 8)
 {
 if (p->bitsPerSample == 8)
            size = dataSize;
 else
 /*lint -e{704} use shift for performance */
            size = dataSize >> 1;
 if (p->loopLength)
            size++;
 }

 else
 {
 if (p->bitsPerSample == 16)
            size = dataSize;
 else
 /*lint -e{703} use shift for performance */
            size = dataSize << 1;
 if (p->loopLength)
            size += 2;
 }

 /* for first pass, add size to wave pool size and return */
 if (pDLSData->pDLS == NULL)
 {
        pDLSData->wavePoolSize += (EAS_U32) size;
 return EAS_SUCCESS;
 }

 /* allocate memory and read in the sample data */
    pSample = pDLSData->pDLS->pDLSSamples + pDLSData->wavePoolOffset;
    pDLSData->pDLS->pDLSSampleOffsets[waveIndex] = pDLSData->wavePoolOffset;
    pDLSData->pDLS->pDLSSampleLen[waveIndex] = (EAS_U32) size;
    pDLSData->wavePoolOffset += (EAS_U32) size;
 if (pDLSData->wavePoolOffset > pDLSData->wavePoolSize)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "Wave pool exceeded allocation\n"); */ }
 return EAS_ERROR_SOUND_LIBRARY;
 }

 if ((result = Parse_data(pDLSData, dataPos, dataSize, p, pSample)) != EAS_SUCCESS)
 return result;

 return EAS_SUCCESS;
}
