 IW_IMPL(unsigned int) iw_get_ui16be(const iw_byte *b)
 {
	return ((unsigned int)b[0]<<8) | (unsigned int)b[1];
 }
