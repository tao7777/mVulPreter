GPMF_ERR IsValidSize(GPMF_stream *ms, uint32_t size) // size is in longs not bytes.
 {
 	if (ms)
 	{
		int32_t nestsize = (int32_t)ms->nest_size[ms->nest_level];
 		if (nestsize == 0 && ms->nest_level == 0)
 			nestsize = ms->buffer_size_longs;
 
		if (size + 2 <= nestsize) return GPMF_OK;
	}
	return GPMF_ERROR_BAD_STRUCTURE;
}
