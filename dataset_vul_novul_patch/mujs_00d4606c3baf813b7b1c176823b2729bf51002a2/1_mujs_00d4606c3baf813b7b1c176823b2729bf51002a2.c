static void Rp_test(js_State *J)
 {
 	js_Regexp *re;
 	const char *text;
 	int opts;
 	Resub m;
 
	re = js_toregexp(J, 0);
	text = js_tostring(J, 1);

	opts = 0;
	if (re->flags & JS_REGEXP_G) {
		if (re->last > strlen(text)) {
			re->last = 0;
			js_pushboolean(J, 0);
			return;
		}
		if (re->last > 0) {
			text += re->last;
			opts |= REG_NOTBOL;
 		}
 	}
 
	if (!js_regexec(re->prog, text, &m, opts)) {
 		if (re->flags & JS_REGEXP_G)
 			re->last = re->last + (m.sub[0].ep - text);
 		js_pushboolean(J, 1);
		return;
	}

	if (re->flags & JS_REGEXP_G)
		re->last = 0;

	js_pushboolean(J, 0);
}
