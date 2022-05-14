 search_make_new(const struct search_state *const state, int n, const char *const base_name) {
 	const size_t base_len = strlen(base_name);
	char need_to_append_dot;
 	struct search_domain *dom;
 
	if (!base_len) return NULL;
	need_to_append_dot = base_name[base_len - 1] == '.' ? 0 : 1;

 	for (dom = state->head; dom; dom = dom->next) {
 		if (!n--) {
 			/* this is the postfix we want */
			/* the actual postfix string is kept at the end of the structure */
			const u8 *const postfix = ((u8 *) dom) + sizeof(struct search_domain);
			const int postfix_len = dom->len;
			char *const newname = (char *) mm_malloc(base_len + need_to_append_dot + postfix_len + 1);
			if (!newname) return NULL;
			memcpy(newname, base_name, base_len);
			if (need_to_append_dot) newname[base_len] = '.';
			memcpy(newname + base_len + need_to_append_dot, postfix, postfix_len);
			newname[base_len + need_to_append_dot + postfix_len] = 0;
			return newname;
		}
	}

	/* we ran off the end of the list and still didn't find the requested string */
	EVUTIL_ASSERT(0);
	return NULL; /* unreachable; stops warnings in some compilers. */
}
