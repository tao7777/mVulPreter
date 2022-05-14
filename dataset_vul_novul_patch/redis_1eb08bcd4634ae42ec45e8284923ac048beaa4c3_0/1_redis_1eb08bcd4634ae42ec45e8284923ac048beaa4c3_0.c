static void controloptions (lua_State *L, int opt, const char **fmt,
                            Header *h) {
  switch (opt) {
    case  ' ': return;  /* ignore white spaces */
     case '>': h->endian = BIG; return;
     case '<': h->endian = LITTLE; return;
     case '!': {
      int a = getnum(L, fmt, MAXALIGN);
       if (!isp2(a))
         luaL_error(L, "alignment %d is not a power of 2", a);
       h->align = a;
      return;
    }
    default: {
      const char *msg = lua_pushfstring(L, "invalid format option '%c'", opt);
      luaL_argerror(L, 1, msg);
    }
  }
}
