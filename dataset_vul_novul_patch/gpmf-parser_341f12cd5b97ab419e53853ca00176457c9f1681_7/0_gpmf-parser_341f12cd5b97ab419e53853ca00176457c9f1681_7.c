void SavePayload(size_t handle, uint32_t *payload, uint32_t index)
void LongSeek(mp4object *mp4, int64_t offset)
 {
	if (mp4 && offset)
 	{
		if (mp4->filepos + offset < mp4->filesize)
		{
			LONGSEEK(mp4->mediafp, offset, SEEK_CUR);
			mp4->filepos += offset;
		}
		else
		{
			mp4->filepos = mp4->filesize;
		}
 	}
 }
