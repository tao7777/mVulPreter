jas_matrix_t *jas_matrix_create(int numrows, int numcols)
jas_matrix_t *jas_matrix_create(jas_matind_t numrows, jas_matind_t numcols)
 {
 	jas_matrix_t *matrix;
	jas_matind_t i;
 	size_t size;
 
 	matrix = 0;

	if (numrows < 0 || numcols < 0) {
		goto error;
	}

	if (!(matrix = jas_malloc(sizeof(jas_matrix_t)))) {
		goto error;
	}
	matrix->flags_ = 0;
	matrix->numrows_ = numrows;
	matrix->numcols_ = numcols;
	matrix->rows_ = 0;
	matrix->maxrows_ = numrows;
	matrix->data_ = 0;
	matrix->datasize_ = 0;

	if (!jas_safe_size_mul(numrows, numcols, &size)) {
		goto error;
	}
	matrix->datasize_ = size;

	if (matrix->maxrows_ > 0) {
		if (!(matrix->rows_ = jas_alloc2(matrix->maxrows_,
		  sizeof(jas_seqent_t *)))) {
			goto error;
		}
	}

	if (matrix->datasize_ > 0) {
		if (!(matrix->data_ = jas_alloc2(matrix->datasize_,
		  sizeof(jas_seqent_t)))) {
			goto error;
		}
	}

	for (i = 0; i < numrows; ++i) {
		matrix->rows_[i] = &matrix->data_[i * matrix->numcols_];
	}

	for (i = 0; i < matrix->datasize_; ++i) {
		matrix->data_[i] = 0;
	}

	matrix->xstart_ = 0;
	matrix->ystart_ = 0;
	matrix->xend_ = matrix->numcols_;
	matrix->yend_ = matrix->numrows_;

	return matrix;

error:
	if (matrix) {
		jas_matrix_destroy(matrix);
	}
	return 0;
}
