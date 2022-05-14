 IW_IMPL(unsigned int) iw_get_ui32be(const iw_byte *b)
 {
	return ((unsigned int)b[0]<<24) | ((unsigned int)b[1]<<16) |
		((unsigned int)b[2]<<8) | (unsigned int)b[3];
 }
