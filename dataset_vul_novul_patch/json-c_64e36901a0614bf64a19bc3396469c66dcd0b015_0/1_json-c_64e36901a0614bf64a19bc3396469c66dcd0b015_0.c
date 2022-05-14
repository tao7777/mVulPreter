 unsigned long lh_char_hash(const void *k)
 {
	unsigned int h = 0;
	const char* data = (const char*)k;
	while( *data!=0 ) h = h*129 + (unsigned int)(*data++) + LH_PRIME;
 
	return h;
 }
