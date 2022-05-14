 cpStripToTile(uint8* out, uint8* in,
    uint32 rows, uint32 cols, int outskew, int inskew)
 {
 	while (rows-- > 0) {
 		uint32 j = cols;
		while (j-- > 0)
			*out++ = *in++;
		out += outskew;
		in += inskew;
	}
}
