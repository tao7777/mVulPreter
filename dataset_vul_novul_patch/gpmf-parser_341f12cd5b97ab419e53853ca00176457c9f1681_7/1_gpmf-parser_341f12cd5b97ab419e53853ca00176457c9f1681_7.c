void SavePayload(size_t handle, uint32_t *payload, uint32_t index)
 {
	mp4object *mp4 = (mp4object *)handle;
	if (mp4 == NULL) return;
	uint32_t *MP4buffer = NULL;
	if (index < mp4->indexcount && mp4->mediafp && payload)
 	{
		LONGSEEK(mp4->mediafp, mp4->metaoffsets[index], SEEK_SET);
		fwrite(payload, 1, mp4->metasizes[index], mp4->mediafp);
 	}
	return;
 }
