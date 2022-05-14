EAS_RESULT DLSParser (EAS_HW_DATA_HANDLE hwInstData, EAS_FILE_HANDLE fileHandle, EAS_I32 offset, EAS_DLSLIB_HANDLE *ppDLS)
{
    EAS_RESULT result;
    SDLS_SYNTHESIZER_DATA dls;
    EAS_U32 temp;
    EAS_I32 pos;
    EAS_I32 chunkPos;
    EAS_I32 size;
    EAS_I32 instSize;
    EAS_I32 rgnPoolSize;
    EAS_I32 artPoolSize;
    EAS_I32 waveLenSize;
    EAS_I32 endDLS;
    EAS_I32 wvplPos;
    EAS_I32 wvplSize;
    EAS_I32 linsPos;
    EAS_I32 linsSize;
    EAS_I32 ptblPos;
    EAS_I32 ptblSize;
 void *p;

 /* zero counts and pointers */
    EAS_HWMemSet(&dls, 0, sizeof(dls));

 /* save file handle and hwInstData to save copying pointers around */
    dls.hwInstData = hwInstData;
    dls.fileHandle = fileHandle;

 /* NULL return value in case of error */
 *ppDLS = NULL;

 /* seek to start of DLS and read in RIFF tag and set processor endian flag */
 if ((result = EAS_HWFileSeek(dls.hwInstData, dls.fileHandle, offset)) != EAS_SUCCESS)
 return result;
 if ((result = EAS_HWReadFile(dls.hwInstData, dls.fileHandle, &temp, sizeof(temp), &size)) != EAS_SUCCESS)
 return result;

 /* check for processor endian-ness */
    dls.bigEndian = (temp == CHUNK_RIFF);

 /* first chunk should be DLS */
    pos = offset;
 if ((result = NextChunk(&dls, &pos, &temp, &size)) != EAS_SUCCESS)
 return result;
 if (temp != CHUNK_DLS)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "Expected DLS chunk, got %08lx\n", temp); */ }
 return EAS_ERROR_FILE_FORMAT;
 }

 /* no instrument or wavepool chunks */
    linsSize = wvplSize = ptblSize = linsPos = wvplPos = ptblPos = 0;

 /* scan the chunks in the DLS list */
    endDLS = offset + size;
    pos = offset + 12;
 while (pos < endDLS)
 {
        chunkPos = pos;

 /* get the next chunk type */
 if ((result = NextChunk(&dls, &pos, &temp, &size)) != EAS_SUCCESS)
 return result;

 /* parse useful chunks */
 switch (temp)
 {
 case CHUNK_CDL:
 if ((result = Parse_cdl(&dls, size, &temp)) != EAS_SUCCESS)
 return result;
 if (!temp)
 return EAS_ERROR_UNRECOGNIZED_FORMAT;
 break;

 case CHUNK_LINS:
                linsPos = chunkPos + 12;
                linsSize = size - 4;
 break;

 case CHUNK_WVPL:
                wvplPos = chunkPos + 12;
                wvplSize = size - 4;
 break;

 case CHUNK_PTBL:
                ptblPos = chunkPos + 8;
                ptblSize = size - 4;
 break;

 default:
 break;
 }
 }

 /* must have a lins chunk */
 if (linsSize == 0)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "No lins chunk found"); */ }
 return EAS_ERROR_UNRECOGNIZED_FORMAT;
 }

 /* must have a wvpl chunk */
 if (wvplSize == 0)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "No wvpl chunk found"); */ }
 return EAS_ERROR_UNRECOGNIZED_FORMAT;
 }

 /* must have a ptbl chunk */
 if ((ptblSize == 0) || (ptblSize > DLS_MAX_WAVE_COUNT * sizeof(POOLCUE) + sizeof(POOLTABLE)))
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "No ptbl chunk found"); */ }
 return EAS_ERROR_UNRECOGNIZED_FORMAT;
 }

 /* pre-parse the wave pool chunk */
 if ((result = Parse_ptbl(&dls, ptblPos, wvplPos, wvplSize)) != EAS_SUCCESS)
 return result;

 /* limit check  */
 if ((dls.waveCount == 0) || (dls.waveCount > DLS_MAX_WAVE_COUNT))
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "DLS file contains invalid #waves [%u]\n", dls.waveCount); */ }
 return EAS_ERROR_FILE_FORMAT;
 }

 /* allocate memory for wsmp data */
    dls.wsmpData = EAS_HWMalloc(dls.hwInstData, (EAS_I32) (sizeof(S_WSMP_DATA) * dls.waveCount));
 if (dls.wsmpData == NULL)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "EAS_HWMalloc for wsmp data failed\n"); */ }
 return EAS_ERROR_MALLOC_FAILED;
 }
    EAS_HWMemSet(dls.wsmpData, 0, (EAS_I32) (sizeof(S_WSMP_DATA) * dls.waveCount));

 /* pre-parse the lins chunk */
    result = Parse_lins(&dls, linsPos, linsSize);
 if (result == EAS_SUCCESS)
 {

 /* limit check  */
 if ((dls.regionCount == 0) || (dls.regionCount > DLS_MAX_REGION_COUNT))
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "DLS file contains invalid #regions [%u]\n", dls.regionCount); */ }
 return EAS_ERROR_FILE_FORMAT;
 }

 /* limit check  */
 if ((dls.artCount == 0) || (dls.artCount > DLS_MAX_ART_COUNT))
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "DLS file contains invalid #articulations [%u]\n", dls.regionCount); */ }
 return EAS_ERROR_FILE_FORMAT;
 }

 /* limit check  */
 if ((dls.instCount == 0) || (dls.instCount > DLS_MAX_INST_COUNT))
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "DLS file contains invalid #instruments [%u]\n", dls.instCount); */ }
 return EAS_ERROR_FILE_FORMAT;
 }

 /* Allocate memory for the converted DLS data */
 /* calculate size of instrument data */
        instSize = (EAS_I32) (sizeof(S_PROGRAM) * dls.instCount);

 /* calculate size of region pool */
        rgnPoolSize = (EAS_I32) (sizeof(S_DLS_REGION) * dls.regionCount);

 /* calculate size of articulation pool, add one for default articulation */
        dls.artCount++;
        artPoolSize = (EAS_I32) (sizeof(S_DLS_ARTICULATION) * dls.artCount);

 /* calculate size of wave length and offset arrays */
        waveLenSize = (EAS_I32) (dls.waveCount * sizeof(EAS_U32));

 /* calculate final memory size */
        size = (EAS_I32) sizeof(S_EAS) + instSize + rgnPoolSize + artPoolSize + (2 * waveLenSize) + (EAS_I32) dls.wavePoolSize;
 if (size <= 0) {
 return EAS_ERROR_FILE_FORMAT;
 }

 /* allocate the main EAS chunk */
        dls.pDLS = EAS_HWMalloc(dls.hwInstData, size);
 if (dls.pDLS == NULL)
 {
 { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "EAS_HWMalloc failed for DLS memory allocation size %ld\n", size); */ }
 return EAS_ERROR_MALLOC_FAILED;
 }
        EAS_HWMemSet(dls.pDLS, 0, size);
        dls.pDLS->refCount = 1;
        p = PtrOfs(dls.pDLS, sizeof(S_EAS));

 /* setup pointer to programs */
        dls.pDLS->numDLSPrograms = (EAS_U16) dls.instCount;
        dls.pDLS->pDLSPrograms = p;
        p = PtrOfs(p, instSize);

 /* setup pointer to regions */
        dls.pDLS->pDLSRegions = p;
        dls.pDLS->numDLSRegions = (EAS_U16) dls.regionCount;
        p = PtrOfs(p, rgnPoolSize);

 /* setup pointer to articulations */
        dls.pDLS->numDLSArticulations = (EAS_U16) dls.artCount;
        dls.pDLS->pDLSArticulations = p;
        p = PtrOfs(p, artPoolSize);

 /* setup pointer to wave length table */
        dls.pDLS->numDLSSamples = (EAS_U16) dls.waveCount;
        dls.pDLS->pDLSSampleLen = p;
        p = PtrOfs(p, waveLenSize);

 /* setup pointer to wave offsets table */
        dls.pDLS->pDLSSampleOffsets = p;
        p = PtrOfs(p, waveLenSize);

 /* setup pointer to wave pool */
        dls.pDLS->pDLSSamples = p;

 /* clear filter flag */
        dls.filterUsed = EAS_FALSE;

 /* parse the wave pool and load samples */
        result = Parse_ptbl(&dls, ptblPos, wvplPos, wvplSize);

     }
 
     /* create the default articulation */
    if (dls.pDLS) {
        Convert_art(&dls, &defaultArt, 0);
        dls.artCount = 1;
    }
 
     /* parse the lins chunk and load instruments */
     dls.regionCount = dls.instCount = 0;
 if (result == EAS_SUCCESS)
        result = Parse_lins(&dls, linsPos, linsSize);

 /* clean up any temporary objects that were allocated */
 if (dls.wsmpData)
        EAS_HWFree(dls.hwInstData, dls.wsmpData);

 /* if successful, return a pointer to the EAS collection */
 if (result == EAS_SUCCESS)
 {
 *ppDLS = dls.pDLS;
#ifdef _DEBUG_DLS
 DumpDLS(dls.pDLS);
#endif
 }

 /* something went wrong, deallocate the EAS collection */
 else
 DLSCleanup(dls.hwInstData, dls.pDLS);

 return result;
}
