static BOOL nsc_context_initialize_encode(NSC_CONTEXT* context)
{
	int i;
	UINT32 length;
	UINT32 tempWidth;
	UINT32 tempHeight;
	tempWidth = ROUND_UP_TO(context->width, 8);
	tempHeight = ROUND_UP_TO(context->height, 2);
	/* The maximum length a decoded plane can reach in all cases */
	length = tempWidth * tempHeight + 16;

	if (length > context->priv->PlaneBuffersLength)
	{
 		for (i = 0; i < 5; i++)
 		{
 			BYTE* tmp = (BYTE*) realloc(context->priv->PlaneBuffers[i], length);
 			if (!tmp)
 				goto fail;
 
			context->priv->PlaneBuffers[i] = tmp;
		}

		context->priv->PlaneBuffersLength = length;
	}

	if (context->ChromaSubsamplingLevel)
	{
		context->OrgByteCount[0] = tempWidth * context->height;
		context->OrgByteCount[1] = tempWidth * tempHeight / 4;
		context->OrgByteCount[2] = tempWidth * tempHeight / 4;
		context->OrgByteCount[3] = context->width * context->height;
	}
	else
	{
		context->OrgByteCount[0] = context->width * context->height;
		context->OrgByteCount[1] = context->width * context->height;
		context->OrgByteCount[2] = context->width * context->height;
		context->OrgByteCount[3] = context->width * context->height;
	}

	return TRUE;
fail:

	if (length > context->priv->PlaneBuffersLength)
	{
		for (i = 0; i < 5; i++)
			free(context->priv->PlaneBuffers[i]);
	}

 	return FALSE;
 }
