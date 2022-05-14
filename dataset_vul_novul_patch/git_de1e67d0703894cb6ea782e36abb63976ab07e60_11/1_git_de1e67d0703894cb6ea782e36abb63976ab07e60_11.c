char *path_name(struct strbuf *path, const char *name)
 {
	struct strbuf ret = STRBUF_INIT;
	if (path)
		strbuf_addbuf(&ret, path);
	strbuf_addstr(&ret, name);
	return strbuf_detach(&ret, NULL);
}
