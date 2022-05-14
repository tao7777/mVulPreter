 IW_IMPL(unsigned int) iw_get_ui16le(const iw_byte *b)
 {
	return b[0] | (b[1]<<8);
 }
