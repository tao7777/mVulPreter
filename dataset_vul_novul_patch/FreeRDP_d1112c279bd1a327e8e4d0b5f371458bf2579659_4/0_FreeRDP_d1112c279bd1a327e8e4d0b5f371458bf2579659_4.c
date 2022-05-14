void nsc_encode(NSC_CONTEXT* context, const BYTE* bmpdata, UINT32 rowstride)
BOOL nsc_encode(NSC_CONTEXT* context, const BYTE* bmpdata, UINT32 rowstride)
 {
	if (!context || !bmpdata || (rowstride == 0))
		return FALSE;

	if (!nsc_encode_argb_to_aycocg(context, bmpdata, rowstride))
		return FALSE;
 
 	if (context->ChromaSubsamplingLevel)
 	{
		if (!nsc_encode_subsampling(context))
			return FALSE;
 	}

	return TRUE;
 }
