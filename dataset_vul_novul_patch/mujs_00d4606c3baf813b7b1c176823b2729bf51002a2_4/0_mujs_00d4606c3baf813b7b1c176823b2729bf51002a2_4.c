static void Sp_split_regexp(js_State *J)
{
	js_Regexp *re;
	const char *text;
	int limit, len, k;
	const char *p, *a, *b, *c, *e;
	Resub m;

	text = checkstring(J, 0);
	re = js_toregexp(J, 1);
	limit = js_isdefined(J, 2) ? js_tointeger(J, 2) : 1 << 30;

	js_newarray(J);
	len = 0;

	e = text + strlen(text);
 
 	/* splitting the empty string */
 	if (e == text) {
		if (js_doregexec(J, re->prog, text, &m, 0)) {
 			if (len == limit) return;
 			js_pushliteral(J, "");
 			js_setindex(J, -2, 0);
		}
		return;
	}
 
 	p = a = text;
 	while (a < e) {
		if (js_doregexec(J, re->prog, a, &m, a > text ? REG_NOTBOL : 0))
 			break; /* no match */
 
 		b = m.sub[0].sp;
		c = m.sub[0].ep;

		/* empty string at end of last match */
		if (b == p) {
			++a;
			continue;
		}

		if (len == limit) return;
		js_pushlstring(J, p, b - p);
		js_setindex(J, -2, len++);

		for (k = 1; k < m.nsub; ++k) {
			if (len == limit) return;
			js_pushlstring(J, m.sub[k].sp, m.sub[k].ep - m.sub[k].sp);
			js_setindex(J, -2, len++);
		}

		a = p = c;
	}

	if (len == limit) return;
	js_pushstring(J, p);
	js_setindex(J, -2, len);
}
