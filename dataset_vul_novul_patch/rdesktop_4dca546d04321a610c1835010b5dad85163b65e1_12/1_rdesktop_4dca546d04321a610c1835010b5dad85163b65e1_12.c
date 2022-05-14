rdpsnddbg_process(STREAM s)
{
	unsigned int pkglen;
 	static char *rest = NULL;
 	char *buf;
 
 	pkglen = s->end - s->p;
 	/* str_handle_lines requires null terminated strings */
 	buf = (char *) xmalloc(pkglen + 1);
	STRNCPY(buf, (char *) s->p, pkglen + 1);

	str_handle_lines(buf, &rest, rdpsnddbg_line_handler, NULL);

	xfree(buf);
}
