 IW_IMPL(int) iw_get_i32le(const iw_byte *b)
 {
	return (iw_int32)(iw_uint32)((unsigned int)b[0] | ((unsigned int)b[1]<<8) |
		((unsigned int)b[2]<<16) | ((unsigned int)b[3]<<24));
 }
