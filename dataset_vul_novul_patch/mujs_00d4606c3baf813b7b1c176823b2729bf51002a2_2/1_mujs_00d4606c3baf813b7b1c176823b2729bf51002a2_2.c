static void Sp_replace_regexp(js_State *J)
{
	js_Regexp *re;
	const char *source, *s, *r;
	js_Buffer *sb = NULL;
	int n, x;
	Resub m;

 	source = checkstring(J, 0);
 	re = js_toregexp(J, 1);
 
	if (js_regexec(re->prog, source, &m, 0)) {
 		js_copy(J, 0);
 		return;
 	}

	re->last = 0;

loop:
	s = m.sub[0].sp;
	n = m.sub[0].ep - m.sub[0].sp;

	if (js_iscallable(J, 2)) {
		js_copy(J, 2);
		js_pushundefined(J);
		for (x = 0; m.sub[x].sp; ++x) /* arg 0..x: substring and subexps that matched */
			js_pushlstring(J, m.sub[x].sp, m.sub[x].ep - m.sub[x].sp);
		js_pushnumber(J, s - source); /* arg x+2: offset within search string */
		js_copy(J, 0); /* arg x+3: search string */
		js_call(J, 2 + x);
		r = js_tostring(J, -1);
		js_putm(J, &sb, source, s);
		js_puts(J, &sb, r);
		js_pop(J, 1);
	} else {
		r = js_tostring(J, 2);
		js_putm(J, &sb, source, s);
		while (*r) {
			if (*r == '$') {
				switch (*(++r)) {
				case 0: --r; /* end of string; back up */
				/* fallthrough */
				case '$': js_putc(J, &sb, '$'); break;
				case '`': js_putm(J, &sb, source, s); break;
				case '\'': js_puts(J, &sb, s + n); break;
				case '&':
					js_putm(J, &sb, s, s + n);
					break;
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					x = *r - '0';
					if (r[1] >= '0' && r[1] <= '9')
						x = x * 10 + *(++r) - '0';
					if (x > 0 && x < m.nsub) {
						js_putm(J, &sb, m.sub[x].sp, m.sub[x].ep);
					} else {
						js_putc(J, &sb, '$');
						if (x > 10) {
							js_putc(J, &sb, '0' + x / 10);
							js_putc(J, &sb, '0' + x % 10);
						} else {
							js_putc(J, &sb, '0' + x);
						}
					}
					break;
				default:
					js_putc(J, &sb, '$');
					js_putc(J, &sb, *r);
					break;
				}
				++r;
			} else {
				js_putc(J, &sb, *r++);
			}
		}
	}

	if (re->flags & JS_REGEXP_G) {
		source = m.sub[0].ep;
		if (n == 0) {
			if (*source)
				js_putc(J, &sb, *source++);
 			else
 				goto end;
 		}
		if (!js_regexec(re->prog, source, &m, REG_NOTBOL))
 			goto loop;
 	}
 
end:
	js_puts(J, &sb, s + n);
	js_putc(J, &sb, 0);

	if (js_try(J)) {
		js_free(J, sb);
		js_throw(J);
	}
	js_pushstring(J, sb ? sb->s : "");
	js_endtry(J);
	js_free(J, sb);
}
