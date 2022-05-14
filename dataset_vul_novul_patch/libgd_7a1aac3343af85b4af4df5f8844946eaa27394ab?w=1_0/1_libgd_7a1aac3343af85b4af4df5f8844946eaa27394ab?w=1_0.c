static inline int _setEdgePixel(const gdImagePtr src, unsigned int x, unsigned int y, gdFixed coverage, const int bgColor) 
 {
 	const gdFixed f_127 = gd_itofx(127);
 	register int c = src->tpixels[y][x];
	c = c | (( (int) (gd_fxtof(gd_mulfx(coverage, f_127)) + 50.5f)) << 24);
	return _color_blend(bgColor, c);
}
