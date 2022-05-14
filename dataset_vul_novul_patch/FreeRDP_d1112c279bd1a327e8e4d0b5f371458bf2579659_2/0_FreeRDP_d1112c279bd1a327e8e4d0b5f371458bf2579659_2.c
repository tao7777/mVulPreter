static void nsc_rle_decompress_data(NSC_CONTEXT* context)
static BOOL nsc_rle_decompress_data(NSC_CONTEXT* context)
 {
 	UINT16 i;
 	BYTE* rle;
 	UINT32 planeSize;
 	UINT32 originalSize;

	if (!context)
		return FALSE;

 	rle = context->Planes;
 
 	for (i = 0; i < 4; i++)
	{
		originalSize = context->OrgByteCount[i];
 		planeSize = context->PlaneByteCount[i];
 
 		if (planeSize == 0)
		{
			if (context->priv->PlaneBuffersLength < originalSize)
				return FALSE;

 			FillMemory(context->priv->PlaneBuffers[i], originalSize, 0xFF);
		}
 		else if (planeSize < originalSize)
		{
			if (!nsc_rle_decode(rle, context->priv->PlaneBuffers[i], context->priv->PlaneBuffersLength,
			                    originalSize))
				return FALSE;
		}
 		else
		{
			if (context->priv->PlaneBuffersLength < originalSize)
				return FALSE;

 			CopyMemory(context->priv->PlaneBuffers[i], rle, originalSize);
		}
 
 		rle += planeSize;
 	}

	return TRUE;
 }
