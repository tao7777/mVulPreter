double GetGPMFSampleRateAndTimes(size_t handle, GPMF_stream *gs, double rate, uint32_t index, double *in, double *out)
{
	mp4object *mp4 = (mp4object *)handle;
	if (mp4 == NULL) return 0.0;
 
	uint32_t key, insamples;
	uint32_t repeat, outsamples;
	GPMF_stream find_stream;
 
	if (gs == NULL || mp4->metaoffsets == 0 || mp4->indexcount == 0 || mp4->basemetadataduration == 0 || mp4->meta_clockdemon == 0 || in == NULL || out == NULL) return 0.0;
 
	key = GPMF_Key(gs);
	repeat = GPMF_Repeat(gs);
	if (rate == 0.0)
		rate = GetGPMFSampleRate(handle, key, GPMF_SAMPLE_RATE_FAST);
 
	if (rate == 0.0)
	{
		*in = *out = 0.0;
		return 0.0;
 	}
 
	GPMF_CopyState(gs, &find_stream);
	if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TOTAL_SAMPLES, GPMF_CURRENT_LEVEL))
	{
		outsamples = BYTESWAP32(*(uint32_t *)GPMF_RawData(&find_stream));
		insamples = outsamples - repeat;
 
		*in = ((double)insamples / (double)rate);
		*out = ((double)outsamples / (double)rate);
	}
	else
	{
		*in = ((double)index * (double)mp4->basemetadataduration / (double)mp4->meta_clockdemon);
		*out = ((double)(index + 1) * (double)mp4->basemetadataduration / (double)mp4->meta_clockdemon);
	}
 	return rate;
 }
