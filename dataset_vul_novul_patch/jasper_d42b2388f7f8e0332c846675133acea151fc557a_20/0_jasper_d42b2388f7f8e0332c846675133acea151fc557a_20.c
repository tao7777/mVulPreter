 jas_matrix_t *jas_matrix_copy(jas_matrix_t *x)
 {
 	jas_matrix_t *y;
	jas_matind_t i;
	jas_matind_t j;
 	y = jas_matrix_create(x->numrows_, x->numcols_);
 	for (i = 0; i < x->numrows_; ++i) {
 		for (j = 0; j < x->numcols_; ++j) {
			*jas_matrix_getref(y, i, j) = jas_matrix_get(x, i, j);
		}
	}
	return y;
}
