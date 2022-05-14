 void js_RegExp_prototype_exec(js_State *J, js_Regexp *re, const char *text)
 {
	int result;
 	int i;
 	int opts;
 	Resub m;

	opts = 0;
	if (re->flags & JS_REGEXP_G) {
		if (re->last > strlen(text)) {
			re->last = 0;
			js_pushnull(J);
			return;
		}
		if (re->last > 0) {
			text += re->last;
			opts |= REG_NOTBOL;
 		}
 	}
 
	result = js_regexec(re->prog, text, &m, opts);
	if (result < 0)
		js_error(J, "regexec failed");
	if (result == 0) {
 		js_newarray(J);
 		js_pushstring(J, text);
 		js_setproperty(J, -2, "input");
		js_pushnumber(J, js_utfptrtoidx(text, m.sub[0].sp));
		js_setproperty(J, -2, "index");
		for (i = 0; i < m.nsub; ++i) {
			js_pushlstring(J, m.sub[i].sp, m.sub[i].ep - m.sub[i].sp);
			js_setindex(J, -2, i);
		}
		if (re->flags & JS_REGEXP_G)
			re->last = re->last + (m.sub[0].ep - text);
		return;
	}

	if (re->flags & JS_REGEXP_G)
		re->last = 0;

	js_pushnull(J);
}
