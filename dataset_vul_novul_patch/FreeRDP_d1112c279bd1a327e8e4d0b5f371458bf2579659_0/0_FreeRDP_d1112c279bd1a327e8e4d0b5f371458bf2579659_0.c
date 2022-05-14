BOOL nsc_process_message(NSC_CONTEXT* context, UINT16 bpp,
                         UINT32 width, UINT32 height,
                         const BYTE* data, UINT32 length,
                         BYTE* pDstData, UINT32 DstFormat,
                         UINT32 nDstStride,
                         UINT32 nXDst, UINT32 nYDst, UINT32 nWidth,
                         UINT32 nHeight, UINT32 flip)
{
	wStream* s;
	BOOL ret;
	s = Stream_New((BYTE*)data, length);

	if (!s)
		return FALSE;

	if (nDstStride == 0)
		nDstStride = nWidth * GetBytesPerPixel(DstFormat);

	switch (bpp)
	{
		case 32:
			context->format = PIXEL_FORMAT_BGRA32;
			break;

		case 24:
			context->format = PIXEL_FORMAT_BGR24;
			break;

		case 16:
			context->format = PIXEL_FORMAT_BGR16;
			break;

		case 8:
			context->format = PIXEL_FORMAT_RGB8;
			break;

		case 4:
			context->format = PIXEL_FORMAT_A4;
			break;

		default:
			Stream_Free(s, TRUE);
			return FALSE;
	}

	context->width = width;
	context->height = height;
	ret = nsc_context_initialize(context, s);
	Stream_Free(s, FALSE);

	if (!ret)
 		return FALSE;
 
 	/* RLE decode */
	{
		BOOL rc;
		PROFILER_ENTER(context->priv->prof_nsc_rle_decompress_data)
		rc = nsc_rle_decompress_data(context);
		PROFILER_EXIT(context->priv->prof_nsc_rle_decompress_data)

		if (!rc)
			return FALSE;
	}
 	/* Colorloss recover, Chroma supersample and AYCoCg to ARGB Conversion in one step */
	{
		BOOL rc;
		PROFILER_ENTER(context->priv->prof_nsc_decode)
		rc = context->decode(context);
		PROFILER_EXIT(context->priv->prof_nsc_decode)

		if (!rc)
			return FALSE;
	}
 
 	if (!freerdp_image_copy(pDstData, DstFormat, nDstStride, nXDst, nYDst,
 	                        width, height, context->BitmapData,
	                        PIXEL_FORMAT_BGRA32, 0, 0, 0, NULL, flip))
		return FALSE;

	return TRUE;
}
