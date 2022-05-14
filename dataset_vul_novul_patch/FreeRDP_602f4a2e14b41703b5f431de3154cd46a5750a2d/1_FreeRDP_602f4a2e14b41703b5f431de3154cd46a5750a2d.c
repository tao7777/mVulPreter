int zgfx_decompress(ZGFX_CONTEXT* zgfx, const BYTE* pSrcData, UINT32 SrcSize, BYTE** ppDstData,
                    UINT32* pDstSize, UINT32 flags)
 {
 	int status = -1;
 	BYTE descriptor;
 	wStream* stream = Stream_New((BYTE*)pSrcData, SrcSize);
 	if (!stream)
 		return -1;
 
	if (Stream_GetRemainingLength(stream) < 1)
		goto fail;

	Stream_Read_UINT8(stream, descriptor); /* descriptor (1 byte) */

	if (descriptor == ZGFX_SEGMENTED_SINGLE)
	{
		if (!zgfx_decompress_segment(zgfx, stream, Stream_GetRemainingLength(stream)))
 			goto fail;
 
 		*ppDstData = NULL;
 		if (zgfx->OutputCount > 0)
 			*ppDstData = (BYTE*) malloc(zgfx->OutputCount);
 
		if (!*ppDstData)
			goto fail;

		*pDstSize = zgfx->OutputCount;
		CopyMemory(*ppDstData, zgfx->OutputBuffer, zgfx->OutputCount);
	}
	else if (descriptor == ZGFX_SEGMENTED_MULTIPART)
	{
		UINT32 segmentSize;
		UINT16 segmentNumber;
 		UINT16 segmentCount;
 		UINT32 uncompressedSize;
 		BYTE* pConcatenated;
 
 		if (Stream_GetRemainingLength(stream) < 6)
 			goto fail;

		Stream_Read_UINT16(stream, segmentCount); /* segmentCount (2 bytes) */
		Stream_Read_UINT32(stream, uncompressedSize); /* uncompressedSize (4 bytes) */

		if (Stream_GetRemainingLength(stream) < segmentCount * sizeof(UINT32))
			goto fail;

		pConcatenated = (BYTE*) malloc(uncompressedSize);

		if (!pConcatenated)
			goto fail;

		*ppDstData = pConcatenated;
		*pDstSize = uncompressedSize;

		for (segmentNumber = 0; segmentNumber < segmentCount; segmentNumber++)
		{
			if (Stream_GetRemainingLength(stream) < sizeof(UINT32))
				goto fail;

			Stream_Read_UINT32(stream, segmentSize); /* segmentSize (4 bytes) */

 			if (!zgfx_decompress_segment(zgfx, stream, segmentSize))
 				goto fail;
 
 			CopyMemory(pConcatenated, zgfx->OutputBuffer, zgfx->OutputCount);
 			pConcatenated += zgfx->OutputCount;
 		}
 	}
 	else
	{
		goto fail;
	}

	status = 1;
fail:
	Stream_Free(stream, FALSE);
	return status;
}
