static size_t optsize (lua_State *L, char opt, const char **fmt) {
  switch (opt) {
    case 'B': case 'b': return sizeof(char);
    case 'H': case 'h': return sizeof(short);
    case 'L': case 'l': return sizeof(long);
    case 'T': return sizeof(size_t);
     case 'f':  return sizeof(float);
     case 'd':  return sizeof(double);
     case 'x': return 1;
    case 'c': return getnum(L, fmt, 1);
     case 'i': case 'I': {
      int sz = getnum(L, fmt, sizeof(int));
       if (sz > MAXINTSIZE)
         luaL_error(L, "integral size %d is larger than limit of %d",
                        sz, MAXINTSIZE);
      return sz;
    }
    default: return 0;  /* other cases do not need alignment */
  }
}
