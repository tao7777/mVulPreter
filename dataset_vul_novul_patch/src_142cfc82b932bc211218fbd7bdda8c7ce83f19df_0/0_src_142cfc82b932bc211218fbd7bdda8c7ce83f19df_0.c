parse_range(char *str, size_t file_sz, int *nranges)
int
parse_ranges(struct client *clt, char *str, size_t file_sz)
 {
 	int			 i = 0;
 	char			*p, *q;
	struct range_data	*r = &clt->clt_ranges;

	memset(r, 0, sizeof(*r));
 
 	/* Extract range unit */
 	if ((p = strchr(str, '=')) == NULL)
		return (-1);
 
 	*p++ = '\0';
 	/* Check if it's a bytes range spec */
 	if (strcmp(str, "bytes") != 0)
		return (-1);
 
 	while ((q = strchr(p, ',')) != NULL) {
 		*q++ = '\0';
 
 		/* Extract start and end positions */
		if (parse_range_spec(p, file_sz, &r->range[i]) == 0)
 			continue;
 
 		i++;
		if (i == SERVER_MAX_RANGES)
			return (-1);
 
 		p = q;
 	}
 
	if (parse_range_spec(p, file_sz, &r->range[i]) != 0)
 		i++;
 
	r->range_total = file_sz;
	r->range_count = i;
	return (i);
 }
