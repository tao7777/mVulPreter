static BOOL gdi_Bitmap_Decompress(rdpContext* context, rdpBitmap* bitmap,
                                  const BYTE* pSrcData, UINT32 DstWidth, UINT32 DstHeight,
                                  UINT32 bpp, UINT32 length, BOOL compressed,
                                  UINT32 codecId)
 {
 	UINT32 SrcSize = length;
 	rdpGdi* gdi = context->gdi;
	UINT32 size = DstWidth * DstHeight;
 	bitmap->compressed = FALSE;
 	bitmap->format = gdi->dstFormat;

	if ((GetBytesPerPixel(bitmap->format) == 0) ||
	    (DstWidth == 0) || (DstHeight == 0) || (DstWidth > UINT32_MAX / DstHeight) ||
	    (size > (UINT32_MAX / GetBytesPerPixel(bitmap->format))))
		return FALSE;

	size *= GetBytesPerPixel(bitmap->format);
	bitmap->length = size;
 	bitmap->data = (BYTE*) _aligned_malloc(bitmap->length, 16);
 
 	if (!bitmap->data)
		return FALSE;

	if (compressed)
	{
		if (bpp < 32)
		{
			if (!interleaved_decompress(context->codecs->interleaved,
			                            pSrcData, SrcSize,
			                            DstWidth, DstHeight,
			                            bpp,
			                            bitmap->data, bitmap->format,
			                            0, 0, 0, DstWidth, DstHeight,
			                            &gdi->palette))
				return FALSE;
		}
		else
		{
			if (!planar_decompress(context->codecs->planar, pSrcData, SrcSize,
			                       DstWidth, DstHeight,
			                       bitmap->data, bitmap->format, 0, 0, 0,
			                       DstWidth, DstHeight, TRUE))
				return FALSE;
		}
	}
	else
	{
		const UINT32 SrcFormat = gdi_get_pixel_format(bpp);
		const size_t sbpp = GetBytesPerPixel(SrcFormat);
		const size_t dbpp = GetBytesPerPixel(bitmap->format);

		if ((sbpp == 0) || (dbpp == 0))
			return FALSE;
		else
		{
			const size_t dstSize = SrcSize * dbpp / sbpp;

			if (dstSize  < bitmap->length)
				return FALSE;
		}

		if (!freerdp_image_copy(bitmap->data, bitmap->format, 0, 0, 0,
		                        DstWidth, DstHeight, pSrcData, SrcFormat,
		                        0, 0, 0, &gdi->palette, FREERDP_FLIP_VERTICAL))
			return FALSE;
	}

	return TRUE;
}
