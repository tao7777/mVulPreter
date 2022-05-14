 jas_matrix_t *jas_seq2d_input(FILE *in)
 {
 	jas_matrix_t *matrix;
	int i;
	int j;
 	long x;
	int numrows;
	int numcols;
	int xoff;
	int yoff;
	if (fscanf(in, "%d %d", &xoff, &yoff) != 2)
 		return 0;
	if (fscanf(in, "%d %d", &numcols, &numrows) != 2)
 		return 0;
	if (!(matrix = jas_seq2d_create(xoff, yoff, xoff + numcols, yoff + numrows)))
 		return 0;
 
 	if (jas_matrix_numrows(matrix) != numrows ||
 	  jas_matrix_numcols(matrix) != numcols) {
		abort();
	}

	/* Get matrix data. */
	for (i = 0; i < jas_matrix_numrows(matrix); i++) {
		for (j = 0; j < jas_matrix_numcols(matrix); j++) {
			if (fscanf(in, "%ld", &x) != 1) {
				jas_matrix_destroy(matrix);
				return 0;
			}
			jas_matrix_set(matrix, i, j, JAS_CAST(jas_seqent_t, x));
		}
	}

	return matrix;
}
