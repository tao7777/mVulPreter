 IW_IMPL(unsigned int) iw_get_ui16be(const iw_byte *b)
 {
	return (b[0]<<8) | b[1];
 }
