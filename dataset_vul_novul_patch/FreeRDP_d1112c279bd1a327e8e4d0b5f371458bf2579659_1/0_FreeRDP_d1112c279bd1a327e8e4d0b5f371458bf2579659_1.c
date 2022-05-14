static void nsc_rle_decode(BYTE* in, BYTE* out, UINT32 originalSize)
static BOOL nsc_rle_decode(BYTE* in, BYTE* out, UINT32 outSize, UINT32 originalSize)
 {
 	UINT32 len;
 	UINT32 left;
	BYTE value;
	left = originalSize;

	while (left > 4)
	{
		value = *in++;
 
 		if (left == 5)
 		{
			if (outSize < 1)
				return FALSE;

			outSize--;
 			*out++ = value;
 			left--;
 		}
		else if (value == *in)
		{
			in++;

			if (*in < 0xFF)
			{
				len = (UINT32) * in++;
				len += 2;
			}
			else
			{
				in++;
				len = *((UINT32*) in);
 				in += 4;
 			}
 
			if (outSize < len)
				return FALSE;

			outSize -= len;
 			FillMemory(out, len, value);
 			out += len;
 			left -= len;
 		}
 		else
 		{
			if (outSize < 1)
				return FALSE;

			outSize--;
 			*out++ = value;
 			left--;
 		}
 	}
 
	if ((outSize < 4) || (left < 4))
		return FALSE;

	memcpy(out, in, 4);
	return TRUE;
 }
