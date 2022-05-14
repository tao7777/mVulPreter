seamless_process(STREAM s)
 {
 	unsigned int pkglen;
 	char *buf;
	struct stream packet = *s;

	if (!s_check(s))
	{
		rdp_protocol_error("seamless_process(), stream is in unstable state", &packet);
	}
 
 	pkglen = s->end - s->p;
 	/* str_handle_lines requires null terminated strings */
	buf = xmalloc(pkglen + 1);
	STRNCPY(buf, (char *) s->p, pkglen + 1);
	str_handle_lines(buf, &seamless_rest, seamless_line_handler, NULL);

	xfree(buf);
}
