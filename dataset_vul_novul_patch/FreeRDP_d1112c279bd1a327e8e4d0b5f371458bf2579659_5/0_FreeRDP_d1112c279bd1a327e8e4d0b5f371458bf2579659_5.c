static void nsc_encode_argb_to_aycocg(NSC_CONTEXT* context, const BYTE* data,
static BOOL nsc_encode_argb_to_aycocg(NSC_CONTEXT* context, const BYTE* data,
                                       UINT32 scanline)
 {
 	UINT16 x;
	UINT16 y;
	UINT16 rw;
	BYTE ccl;
	const BYTE* src;
	BYTE* yplane = NULL;
	BYTE* coplane = NULL;
	BYTE* cgplane = NULL;
	BYTE* aplane = NULL;
	INT16 r_val;
	INT16 g_val;
 	INT16 b_val;
 	BYTE a_val;
 	UINT32 tempWidth;

	if (!context || data || (scanline == 0))
		return FALSE;

 	tempWidth = ROUND_UP_TO(context->width, 8);
 	rw = (context->ChromaSubsamplingLevel ? tempWidth : context->width);
 	ccl = context->ColorLossLevel;
 
	if (context->priv->PlaneBuffersLength < rw * scanline)
		return FALSE;

	if (rw < scanline * 2)
		return FALSE;

 	for (y = 0; y < context->height; y++)
 	{
 		src = data + (context->height - 1 - y) * scanline;
		yplane = context->priv->PlaneBuffers[0] + y * rw;
		coplane = context->priv->PlaneBuffers[1] + y * rw;
		cgplane = context->priv->PlaneBuffers[2] + y * rw;
		aplane = context->priv->PlaneBuffers[3] + y * context->width;

		for (x = 0; x < context->width; x++)
		{
			switch (context->format)
			{
				case PIXEL_FORMAT_BGRX32:
					b_val = *src++;
					g_val = *src++;
					r_val = *src++;
					src++;
					a_val = 0xFF;
					break;

				case PIXEL_FORMAT_BGRA32:
					b_val = *src++;
					g_val = *src++;
					r_val = *src++;
					a_val = *src++;
					break;

				case PIXEL_FORMAT_RGBX32:
					r_val = *src++;
					g_val = *src++;
					b_val = *src++;
					src++;
					a_val = 0xFF;
					break;

				case PIXEL_FORMAT_RGBA32:
					r_val = *src++;
					g_val = *src++;
					b_val = *src++;
					a_val = *src++;
					break;

				case PIXEL_FORMAT_BGR24:
					b_val = *src++;
					g_val = *src++;
					r_val = *src++;
					a_val = 0xFF;
					break;

				case PIXEL_FORMAT_RGB24:
					r_val = *src++;
					g_val = *src++;
					b_val = *src++;
					a_val = 0xFF;
					break;

				case PIXEL_FORMAT_BGR16:
					b_val = (INT16)(((*(src + 1)) & 0xF8) | ((*(src + 1)) >> 5));
					g_val = (INT16)((((*(src + 1)) & 0x07) << 5) | (((*src) & 0xE0) >> 3));
					r_val = (INT16)((((*src) & 0x1F) << 3) | (((*src) >> 2) & 0x07));
					a_val = 0xFF;
					src += 2;
					break;

				case PIXEL_FORMAT_RGB16:
					r_val = (INT16)(((*(src + 1)) & 0xF8) | ((*(src + 1)) >> 5));
					g_val = (INT16)((((*(src + 1)) & 0x07) << 5) | (((*src) & 0xE0) >> 3));
					b_val = (INT16)((((*src) & 0x1F) << 3) | (((*src) >> 2) & 0x07));
					a_val = 0xFF;
					src += 2;
					break;

				case PIXEL_FORMAT_A4:
					{
						int shift;
						BYTE idx;
						shift = (7 - (x % 8));
						idx = ((*src) >> shift) & 1;
						idx |= (((*(src + 1)) >> shift) & 1) << 1;
						idx |= (((*(src + 2)) >> shift) & 1) << 2;
						idx |= (((*(src + 3)) >> shift) & 1) << 3;
						idx *= 3;
						r_val = (INT16) context->palette[idx];
						g_val = (INT16) context->palette[idx + 1];
						b_val = (INT16) context->palette[idx + 2];

						if (shift == 0)
							src += 4;
					}

					a_val = 0xFF;
					break;

				case PIXEL_FORMAT_RGB8:
					{
						int idx = (*src) * 3;
						r_val = (INT16) context->palette[idx];
						g_val = (INT16) context->palette[idx + 1];
						b_val = (INT16) context->palette[idx + 2];
						src++;
					}

					a_val = 0xFF;
					break;

				default:
					r_val = g_val = b_val = a_val = 0;
					break;
			}

			*yplane++ = (BYTE)((r_val >> 2) + (g_val >> 1) + (b_val >> 2));
			/* Perform color loss reduction here */
			*coplane++ = (BYTE)((r_val - b_val) >> ccl);
			*cgplane++ = (BYTE)((-(r_val >> 1) + g_val - (b_val >> 1)) >> ccl);
			*aplane++ = a_val;
		}

		if (context->ChromaSubsamplingLevel && (x % 2) == 1)
		{
			*yplane = *(yplane - 1);
			*coplane = *(coplane - 1);
			*cgplane = *(cgplane - 1);
		}
	}

	if (context->ChromaSubsamplingLevel && (y % 2) == 1)
	{
		yplane = context->priv->PlaneBuffers[0] + y * rw;
		coplane = context->priv->PlaneBuffers[1] + y * rw;
		cgplane = context->priv->PlaneBuffers[2] + y * rw;
		CopyMemory(yplane, yplane - rw, rw);
 		CopyMemory(coplane, coplane - rw, rw);
 		CopyMemory(cgplane, cgplane - rw, rw);
 	}

	return TRUE;
 }
