static void nsc_rle_decode(BYTE* in, BYTE* out, UINT32 originalSize)
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
 
 			FillMemory(out, len, value);
 			out += len;
 			left -= len;
 		}
 		else
 		{
 			*out++ = value;
 			left--;
 		}
 	}
 
	*((UINT32*)out) = *((UINT32*)in);
 }
