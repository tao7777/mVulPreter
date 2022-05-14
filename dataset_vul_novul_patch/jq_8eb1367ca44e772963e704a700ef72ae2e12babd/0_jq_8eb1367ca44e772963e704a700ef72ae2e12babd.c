static pfunc check_literal(struct jv_parser* p) {
  if (p->tokenpos == 0) return 0;

  const char* pattern = 0;
  int plen;
  jv v;
  switch (p->tokenbuf[0]) {
  case 't': pattern = "true"; plen = 4; v = jv_true(); break;
  case 'f': pattern = "false"; plen = 5; v = jv_false(); break;
  case 'n': pattern = "null"; plen = 4; v = jv_null(); break;
  }
  if (pattern) {
    if (p->tokenpos != plen) return "Invalid literal";
    for (int i=0; i<plen; i++)
      if (p->tokenbuf[i] != pattern[i])
        return "Invalid literal";
     TRY(value(p, v));
   } else {
    p->tokenbuf[p->tokenpos] = 0;
     char* end = 0;
     double d = jvp_strtod(&p->dtoa, p->tokenbuf, &end);
     if (end == 0 || *end != 0)
      return "Invalid numeric literal";
    TRY(value(p, jv_number(d)));
  }
  p->tokenpos = 0;
  return 0;
}
