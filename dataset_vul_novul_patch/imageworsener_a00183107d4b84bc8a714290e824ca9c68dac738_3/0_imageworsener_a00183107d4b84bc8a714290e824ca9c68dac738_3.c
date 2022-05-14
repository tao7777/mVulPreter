 IW_IMPL(unsigned int) iw_get_ui16le(const iw_byte *b)
 {
	return (unsigned int)b[0] | ((unsigned int)b[1]<<8);
 }
