poly_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
 	POLYGON    *poly;
 	int			npts;
 	int			size;
 	int			isopen;
 	char	   *s;
 
	if ((npts = pair_count(str, ',')) <= 0)
		ereport(ERROR,
 				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
 			  errmsg("invalid input syntax for type polygon: \"%s\"", str)));
 
	size = offsetof(POLYGON, p[0]) +sizeof(poly->p[0]) * npts;
 	poly = (POLYGON *) palloc0(size);	/* zero any holes */
 
 	SET_VARSIZE(poly, size);
	poly->npts = npts;

	if ((!path_decode(FALSE, npts, str, &isopen, &s, &(poly->p[0])))
		|| (*s != '\0'))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			  errmsg("invalid input syntax for type polygon: \"%s\"", str)));

	make_bound_box(poly);

	PG_RETURN_POLYGON_P(poly);
}
