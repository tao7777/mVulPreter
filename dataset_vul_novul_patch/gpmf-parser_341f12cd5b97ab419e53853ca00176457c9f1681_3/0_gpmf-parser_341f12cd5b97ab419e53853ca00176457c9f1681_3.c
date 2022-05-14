 uint32_t *GetPayload(size_t handle, uint32_t *lastpayload, uint32_t index)
 {
 	mp4object *mp4 = (mp4object *)handle;
	if (mp4 == NULL) return NULL;

	uint32_t *MP4buffer = NULL;
	if (index < mp4->indexcount && mp4->mediafp)
	{
		MP4buffer = (uint32_t *)realloc((void *)lastpayload, mp4->metasizes[index]);
 
 		if (MP4buffer)
 		{
			if (mp4->filesize > mp4->metaoffsets[index]+mp4->metasizes[index])
			{
				LONGSEEK(mp4->mediafp, mp4->metaoffsets[index], SEEK_SET);
				fread(MP4buffer, 1, mp4->metasizes[index], mp4->mediafp);
				mp4->filepos = mp4->metaoffsets[index] + mp4->metasizes[index];
				return MP4buffer;
			}
 		}
 	}
 	return NULL;
 }
