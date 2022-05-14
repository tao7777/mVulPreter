 IW_IMPL(unsigned int) iw_get_ui32le(const iw_byte *b)
 {
	return b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24);
 }
