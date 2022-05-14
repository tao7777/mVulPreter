static BOOL zgfx_decompress_segment(ZGFX_CONTEXT* zgfx, wStream* stream, size_t segmentSize)
{
	BYTE c;
	BYTE flags;
	UINT32 extra = 0;
	int opIndex;
	int haveBits;
	int inPrefix;
 	UINT32 count;
 	UINT32 distance;
 	BYTE* pbSegment;
	size_t cbSegment;
 
	if (!zgfx || !stream)
		return FALSE;

	cbSegment = segmentSize - 1;

	if ((Stream_GetRemainingLength(stream) < segmentSize) || (segmentSize < 1) ||
	    (segmentSize > UINT32_MAX))
 		return FALSE;
 
 	Stream_Read_UINT8(stream, flags); /* header (1 byte) */
	zgfx->OutputCount = 0;
	pbSegment = Stream_Pointer(stream);
	Stream_Seek(stream, cbSegment);

 	if (!(flags & PACKET_COMPRESSED))
 	{
 		zgfx_history_buffer_ring_write(zgfx, pbSegment, cbSegment);

		if (cbSegment > sizeof(zgfx->OutputBuffer))
			return FALSE;

 		CopyMemory(zgfx->OutputBuffer, pbSegment, cbSegment);
 		zgfx->OutputCount = cbSegment;
 		return TRUE;
	}

	zgfx->pbInputCurrent = pbSegment;
	zgfx->pbInputEnd = &pbSegment[cbSegment - 1];
	/* NumberOfBitsToDecode = ((NumberOfBytesToDecode - 1) * 8) - ValueOfLastByte */
	zgfx->cBitsRemaining = 8 * (cbSegment - 1) - *zgfx->pbInputEnd;
	zgfx->cBitsCurrent = 0;
	zgfx->BitsCurrent = 0;

	while (zgfx->cBitsRemaining)
	{
		haveBits = 0;
		inPrefix = 0;

		for (opIndex = 0; ZGFX_TOKEN_TABLE[opIndex].prefixLength != 0; opIndex++)
		{
			while (haveBits < ZGFX_TOKEN_TABLE[opIndex].prefixLength)
			{
				zgfx_GetBits(zgfx, 1);
				inPrefix = (inPrefix << 1) + zgfx->bits;
				haveBits++;
			}

			if (inPrefix == ZGFX_TOKEN_TABLE[opIndex].prefixCode)
			{
				if (ZGFX_TOKEN_TABLE[opIndex].tokenType == 0)
				{
					/* Literal */
					zgfx_GetBits(zgfx, ZGFX_TOKEN_TABLE[opIndex].valueBits);
					c = (BYTE)(ZGFX_TOKEN_TABLE[opIndex].valueBase + zgfx->bits);
					zgfx->HistoryBuffer[zgfx->HistoryIndex] = c;

 					if (++zgfx->HistoryIndex == zgfx->HistoryBufferSize)
 						zgfx->HistoryIndex = 0;
 
					if (zgfx->OutputCount >= sizeof(zgfx->OutputBuffer))
						return FALSE;

 					zgfx->OutputBuffer[zgfx->OutputCount++] = c;
 				}
 				else
				{
					zgfx_GetBits(zgfx, ZGFX_TOKEN_TABLE[opIndex].valueBits);
					distance = ZGFX_TOKEN_TABLE[opIndex].valueBase + zgfx->bits;

					if (distance != 0)
					{
						/* Match */
						zgfx_GetBits(zgfx, 1);

						if (zgfx->bits == 0)
						{
							count = 3;
						}
						else
						{
							count = 4;
							extra = 2;
							zgfx_GetBits(zgfx, 1);

							while (zgfx->bits == 1)
							{
								count *= 2;
								extra++;
								zgfx_GetBits(zgfx, 1);
							}

							zgfx_GetBits(zgfx, extra);
 							count += zgfx->bits;
 						}
 
						if (count > sizeof(zgfx->OutputBuffer) - zgfx->OutputCount)
							return FALSE;

 						zgfx_history_buffer_ring_read(zgfx, distance, &(zgfx->OutputBuffer[zgfx->OutputCount]), count);
 						zgfx_history_buffer_ring_write(zgfx, &(zgfx->OutputBuffer[zgfx->OutputCount]), count);
 						zgfx->OutputCount += count;
					}
					else
					{
						/* Unencoded */
						zgfx_GetBits(zgfx, 15);
						count = zgfx->bits;
 						zgfx->cBitsRemaining -= zgfx->cBitsCurrent;
 						zgfx->cBitsCurrent = 0;
 						zgfx->BitsCurrent = 0;

						if (count > sizeof(zgfx->OutputBuffer) - zgfx->OutputCount)
							return FALSE;

 						CopyMemory(&(zgfx->OutputBuffer[zgfx->OutputCount]), zgfx->pbInputCurrent, count);
 						zgfx_history_buffer_ring_write(zgfx, zgfx->pbInputCurrent, count);
 						zgfx->pbInputCurrent += count;
						zgfx->cBitsRemaining -= (8 * count);
						zgfx->OutputCount += count;
					}
				}

				break;
			}
		}
	}

	return TRUE;
}
