static int b_unpack (lua_State *L) {
  Header h;
  const char *fmt = luaL_checkstring(L, 1);
   size_t ld;
   const char *data = luaL_checklstring(L, 2, &ld);
   size_t pos = luaL_optinteger(L, 3, 1) - 1;
   defaultoptions(&h);
  lua_settop(L, 2);
   while (*fmt) {
     int opt = *fmt++;
     size_t size = optsize(L, opt, &fmt);
     pos += gettoalign(pos, &h, opt, size);
     luaL_argcheck(L, pos+size <= ld, 2, "data string too short");
    luaL_checkstack(L, 1, "too many results");
     switch (opt) {
       case 'b': case 'B': case 'h': case 'H':
       case 'l': case 'L': case 'T': case 'i':  case 'I': {  /* integer types */
         int issigned = islower(opt);
         lua_Number res = getinteger(data+pos, h.endian, issigned, size);
        lua_pushnumber(L, res);
         break;
       }
       case 'x': {
        break;
      }
      case 'f': {
         float f;
         memcpy(&f, data+pos, size);
         correctbytes((char *)&f, sizeof(f), h.endian);
        lua_pushnumber(L, f);
         break;
       }
       case 'd': {
         double d;
         memcpy(&d, data+pos, size);
         correctbytes((char *)&d, sizeof(d), h.endian);
        lua_pushnumber(L, d);
         break;
       }
       case 'c': {
         if (size == 0) {
          if (!lua_isnumber(L, -1))
            luaL_error(L, "format `c0' needs a previous size");
           size = lua_tonumber(L, -1);
          lua_pop(L, 1);
          luaL_argcheck(L, pos+size <= ld, 2, "data string too short");
         }
        lua_pushlstring(L, data+pos, size);
         break;
       }
       case 's': {
         const char *e = (const char *)memchr(data+pos, '\0', ld - pos);
         if (e == NULL)
           luaL_error(L, "unfinished string in data");
         size = (e - (data+pos)) + 1;
        lua_pushlstring(L, data+pos, size - 1);
         break;
       }
       default: controloptions(L, opt, &fmt, &h);
     }
     pos += size;
   }
  lua_pushinteger(L, pos + 1);
  return lua_gettop(L) - 2;
 }
