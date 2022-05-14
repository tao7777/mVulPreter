static void nsc_decode(NSC_CONTEXT* context)
static BOOL nsc_decode(NSC_CONTEXT* context)
 {
 	UINT16 x;
 	UINT16 y;
	UINT16 rw;
	BYTE shift;
	BYTE* bmpdata;
	size_t pos = 0;

	if (!context)
		return FALSE;

	rw = ROUND_UP_TO(context->width, 8);
	shift = context->ColorLossLevel - 1; /* colorloss recovery + YCoCg shift */
	bmpdata = context->BitmapData;

	if (!bmpdata)
		return FALSE;
 
 	for (y = 0; y < context->height; y++)
 	{
		const BYTE* yplane;
		const BYTE* coplane;
		const BYTE* cgplane;
		const BYTE* aplane = context->priv->PlaneBuffers[3] + y * context->width; /* A */

		if (context->ChromaSubsamplingLevel)
		{
			yplane = context->priv->PlaneBuffers[0] + y * rw; /* Y */
			coplane = context->priv->PlaneBuffers[1] + (y >> 1) * (rw >>
			          1); /* Co, supersampled */
			cgplane = context->priv->PlaneBuffers[2] + (y >> 1) * (rw >>
			          1); /* Cg, supersampled */
		}
		else
		{
			yplane = context->priv->PlaneBuffers[0] + y * context->width; /* Y */
			coplane = context->priv->PlaneBuffers[1] + y * context->width; /* Co */
			cgplane = context->priv->PlaneBuffers[2] + y * context->width; /* Cg */
		}

		for (x = 0; x < context->width; x++)
		{
			INT16 y_val = (INT16) * yplane;
			INT16 co_val = (INT16)(INT8)(*coplane << shift);
			INT16 cg_val = (INT16)(INT8)(*cgplane << shift);
 			INT16 r_val = y_val + co_val - cg_val;
 			INT16 g_val = y_val + cg_val;
 			INT16 b_val = y_val - co_val - cg_val;

			if (pos + 4 > context->BitmapDataLength)
				return FALSE;

			pos += 4;
 			*bmpdata++ = MINMAX(b_val, 0, 0xFF);
 			*bmpdata++ = MINMAX(g_val, 0, 0xFF);
 			*bmpdata++ = MINMAX(r_val, 0, 0xFF);
			*bmpdata++ = *aplane;
			yplane++;
			coplane += (context->ChromaSubsamplingLevel ? x % 2 : 1);
			cgplane += (context->ChromaSubsamplingLevel ? x % 2 : 1);
 			aplane++;
 		}
 	}

	return TRUE;
 }
