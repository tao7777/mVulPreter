void nsc_encode(NSC_CONTEXT* context, const BYTE* bmpdata, UINT32 rowstride)
 {
	nsc_encode_argb_to_aycocg(context, bmpdata, rowstride);
 
 	if (context->ChromaSubsamplingLevel)
 	{
		nsc_encode_subsampling(context);
 	}
 }
