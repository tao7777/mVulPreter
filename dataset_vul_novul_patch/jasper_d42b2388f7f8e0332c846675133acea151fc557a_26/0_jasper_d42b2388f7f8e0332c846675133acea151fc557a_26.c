 jas_seq2d_t *jas_seq2d_copy(jas_seq2d_t *x)
 {
 	jas_matrix_t *y;
	jas_matind_t i;
	jas_matind_t j;
 	y = jas_seq2d_create(jas_seq2d_xstart(x), jas_seq2d_ystart(x),
 	  jas_seq2d_xend(x), jas_seq2d_yend(x));
 	assert(y);
	for (i = 0; i < x->numrows_; ++i) {
		for (j = 0; j < x->numcols_; ++j) {
			*jas_matrix_getref(y, i, j) = jas_matrix_get(x, i, j);
		}
	}
	return y;
}
