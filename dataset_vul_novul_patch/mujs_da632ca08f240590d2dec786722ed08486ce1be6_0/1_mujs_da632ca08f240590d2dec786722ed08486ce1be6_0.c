 static void numtostr(js_State *J, const char *fmt, int w, double n)
 {
	char buf[32], *e;
 	sprintf(buf, fmt, w, n);
 	e = strchr(buf, 'e');
 	if (e) {
		int exp = atoi(e+1);
		sprintf(e, "e%+d", exp);
	}
	js_pushstring(J, buf);
}
