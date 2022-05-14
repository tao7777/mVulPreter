seamless_process(STREAM s)
 {
 	unsigned int pkglen;
 	char *buf;
 
 	pkglen = s->end - s->p;
 	/* str_handle_lines requires null terminated strings */
	buf = xmalloc(pkglen + 1);
	STRNCPY(buf, (char *) s->p, pkglen + 1);
	str_handle_lines(buf, &seamless_rest, seamless_line_handler, NULL);

	xfree(buf);
}
