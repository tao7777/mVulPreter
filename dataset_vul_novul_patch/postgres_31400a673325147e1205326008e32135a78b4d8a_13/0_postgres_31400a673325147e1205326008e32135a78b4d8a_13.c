path_poly(PG_FUNCTION_ARGS)
{
	PATH	   *path = PG_GETARG_PATH_P(0);
	POLYGON    *poly;
	int			size;
	int			i;

	/* This is not very consistent --- other similar cases return NULL ... */
	if (!path->closed)
		ereport(ERROR,
 				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
 				 errmsg("open path cannot be converted to polygon")));
 
	/*
	 * Never overflows: the old size fit in MaxAllocSize, and the new size is
	 * just a small constant larger.
	 */
 	size = offsetof(POLYGON, p[0]) +sizeof(poly->p[0]) * path->npts;
 	poly = (POLYGON *) palloc(size);
 
	SET_VARSIZE(poly, size);
	poly->npts = path->npts;

	for (i = 0; i < path->npts; i++)
	{
		poly->p[i].x = path->p[i].x;
		poly->p[i].y = path->p[i].y;
	}

	make_bound_box(poly);

	PG_RETURN_POLYGON_P(poly);
}
