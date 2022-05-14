 static void Np_toString(js_State *J)
 {
	char buf[100];
 	js_Object *self = js_toobject(J, 0);
 	int radix = js_isundefined(J, 1) ? 10 : js_tointeger(J, 1);
 	if (self->type != JS_CNUMBER)
		js_typeerror(J, "not a number");
	if (radix == 10) {
		js_pushstring(J, jsV_numbertostring(J, buf, self->u.number));
		return;
	}
	if (radix < 2 || radix > 36)
		js_rangeerror(J, "invalid radix");

 	/* lame number to string conversion for any radix from 2 to 36 */
 	{
 		static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
 		double number = self->u.number;
 		int sign = self->u.number < 0;
 		js_Buffer *sb = NULL;
		uint64_t u, limit = ((uint64_t)1<<52);

		int ndigits, exp, point;

		if (number == 0) { js_pushstring(J, "0"); return; }
		if (isnan(number)) { js_pushstring(J, "NaN"); return; }
		if (isinf(number)) { js_pushstring(J, sign ? "-Infinity" : "Infinity"); return; }

		if (sign)
			number = -number;

		/* fit as many digits as we want in an int */
		exp = 0;
		while (number * pow(radix, exp) > limit)
			--exp;
		while (number * pow(radix, exp+1) < limit)
			++exp;
		u = number * pow(radix, exp) + 0.5;

		/* trim trailing zeros */
		while (u > 0 && (u % radix) == 0) {
			u /= radix;
			--exp;
		}

		/* serialize digits */
		ndigits = 0;
		while (u > 0) {
			buf[ndigits++] = digits[u % radix];
			u /= radix;
		}
		point = ndigits - exp;

		if (js_try(J)) {
			js_free(J, sb);
			js_throw(J);
		}

		if (sign)
			js_putc(J, &sb, '-');

		if (point <= 0) {
			js_putc(J, &sb, '0');
			js_putc(J, &sb, '.');
			while (point++ < 0)
				js_putc(J, &sb, '0');
			while (ndigits-- > 0)
				js_putc(J, &sb, buf[ndigits]);
		} else {
			while (ndigits-- > 0) {
				js_putc(J, &sb, buf[ndigits]);
				if (--point == 0 && ndigits > 0)
					js_putc(J, &sb, '.');
			}
			while (point-- > 0)
				js_putc(J, &sb, '0');
		}

		js_putc(J, &sb, 0);
		js_pushstring(J, sb->s);

		js_endtry(J);
		js_free(J, sb);
	}
}
