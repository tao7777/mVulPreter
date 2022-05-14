static void Sp_match(js_State *J)
{
	js_Regexp *re;
	const char *text;
	int len;
	const char *a, *b, *c, *e;
	Resub m;

	text = checkstring(J, 0);

	if (js_isregexp(J, 1))
		js_copy(J, 1);
	else if (js_isundefined(J, 1))
		js_newregexp(J, "", 0);
	else
		js_newregexp(J, js_tostring(J, 1), 0);

	re = js_toregexp(J, -1);
	if (!(re->flags & JS_REGEXP_G)) {
		js_RegExp_prototype_exec(J, re, text);
		return;
	}

	re->last = 0;

	js_newarray(J);

	len = 0;
 	a = text;
 	e = text + strlen(text);
 	while (a <= e) {
		if (js_doregexec(J, re->prog, a, &m, a > text ? REG_NOTBOL : 0))
 			break;
 
 		b = m.sub[0].sp;
		c = m.sub[0].ep;

		js_pushlstring(J, b, c - b);
		js_setindex(J, -2, len++);

		a = c;
		if (c - b == 0)
			++a;
	}

	if (len == 0) {
		js_pop(J, 1);
		js_pushnull(J);
	}
}
