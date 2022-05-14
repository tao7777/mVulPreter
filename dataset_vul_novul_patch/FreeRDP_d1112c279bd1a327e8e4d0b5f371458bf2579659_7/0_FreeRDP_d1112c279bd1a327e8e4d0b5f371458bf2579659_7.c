static UINT32 nsc_rle_encode(BYTE* in, BYTE* out, UINT32 originalSize)
static UINT32 nsc_rle_encode(const BYTE* in, BYTE* out, UINT32 originalSize)
 {
 	UINT32 left;
 	UINT32 runlength = 1;
	UINT32 planeSize = 0;
	left = originalSize;

	/**
	 * We quit the loop if the running compressed size is larger than the original.
	 * In such cases data will be sent uncompressed.
	 */
	while (left > 4 && planeSize < originalSize - 4)
	{
		if (left > 5 && *in == *(in + 1))
		{
			runlength++;
		}
		else if (runlength == 1)
		{
			*out++ = *in;
			planeSize++;
		}
		else if (runlength < 256)
		{
			*out++ = *in;
			*out++ = *in;
			*out++ = runlength - 2;
			runlength = 1;
			planeSize += 3;
		}
		else
		{
			*out++ = *in;
			*out++ = *in;
			*out++ = 0xFF;
			*out++ = (runlength & 0x000000FF);
			*out++ = (runlength & 0x0000FF00) >> 8;
			*out++ = (runlength & 0x00FF0000) >> 16;
			*out++ = (runlength & 0xFF000000) >> 24;
			runlength = 1;
			planeSize += 7;
		}

		in++;
		left--;
	}

	if (planeSize < originalSize - 4)
		CopyMemory(out, in, 4);

	planeSize += 4;
	return planeSize;
}
