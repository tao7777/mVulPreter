static void nsc_encode_sse2(NSC_CONTEXT* context, const BYTE* data,
static BOOL nsc_encode_sse2(NSC_CONTEXT* context, const BYTE* data,
                             UINT32 scanline)
 {
 	nsc_encode_argb_to_aycocg_sse2(context, data, scanline);

	if (context->ChromaSubsamplingLevel > 0)
 	{
 		nsc_encode_subsampling_sse2(context);
 	}

	return TRUE;
 }
