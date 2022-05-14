uint32_t GetPayloadTime(size_t handle, uint32_t index, float *in, float *out)
uint32_t GetPayloadTime(size_t handle, uint32_t index, double *in, double *out)
 {
 	mp4object *mp4 = (mp4object *)handle;
	if (mp4 == NULL) return GPMF_ERROR_MEMORY;
 
	if (mp4->metaoffsets == 0 || mp4->basemetadataduration == 0 || mp4->meta_clockdemon == 0 || in == NULL || out == NULL) return GPMF_ERROR_MEMORY;
 
	*in = ((double)index * (double)mp4->basemetadataduration / (double)mp4->meta_clockdemon);
	*out = ((double)(index + 1) * (double)mp4->basemetadataduration / (double)mp4->meta_clockdemon);
	return GPMF_OK;
 }
