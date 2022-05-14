static int read_exceptions(struct pstore *ps,
			   int (*callback)(void *callback_context, chunk_t old,
					   chunk_t new),
			   void *callback_context)
{
	int r, full = 1;

	/*
	 * Keeping reading chunks and inserting exceptions until
	 * we find a partially full area.
	 */
	for (ps->current_area = 0; full; ps->current_area++) {
		r = area_io(ps, READ);
		if (r)
			return r;

		r = insert_exceptions(ps, callback, callback_context, &full);
		if (r)
			return r;
	}
 
 	ps->current_area--;
 
	skip_metadata(ps);

 	return 0;
 }
