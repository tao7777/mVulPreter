poly_path(PG_FUNCTION_ARGS)
{
	POLYGON    *poly = PG_GETARG_POLYGON_P(0);
	PATH	   *path;
 	int			size;
 	int			i;
 
	/*
	 * Never overflows: the old size fit in MaxAllocSize, and the new size is
	 * smaller by a small constant.
	 */
 	size = offsetof(PATH, p[0]) +sizeof(path->p[0]) * poly->npts;
 	path = (PATH *) palloc(size);
 
	SET_VARSIZE(path, size);
	path->npts = poly->npts;
	path->closed = TRUE;
	/* prevent instability in unused pad bytes */
	path->dummy = 0;

	for (i = 0; i < poly->npts; i++)
	{
		path->p[i].x = poly->p[i].x;
		path->p[i].y = poly->p[i].y;
	}

	PG_RETURN_PATH_P(path);
}
