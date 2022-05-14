jas_matrix_t *jas_seq2d_create(int xstart, int ystart, int xend, int yend)
 {
 	jas_matrix_t *matrix;
 	assert(xstart <= xend && ystart <= yend);
	if (!(matrix = jas_matrix_create(yend - ystart, xend - xstart))) {
		return 0;
	}
	matrix->xstart_ = xstart;
	matrix->ystart_ = ystart;
	matrix->xend_ = xend;
	matrix->yend_ = yend;
 	return matrix;
 }
