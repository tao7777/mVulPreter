void jas_seq2d_bindsub(jas_matrix_t *s, jas_matrix_t *s1, int xstart,
  int ystart, int xend, int yend)
 {
 	jas_matrix_bindsub(s, s1, ystart - s1->ystart_, xstart - s1->xstart_,
 	  yend - s1->ystart_ - 1, xend - s1->xstart_ - 1);
 }
