 IW_IMPL(int) iw_get_i32le(const iw_byte *b)
 {
	return (iw_int32)(iw_uint32)(b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24));
 }
