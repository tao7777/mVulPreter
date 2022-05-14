void mp_encode_lua_table_as_array(lua_State *L, mp_buf *buf, int level) {
#if LUA_VERSION_NUM < 502
    size_t len = lua_objlen(L,-1), j;
#else
    size_t len = lua_rawlen(L,-1), j;
 #endif
 
     mp_encode_array(L,buf,len);
     for (j = 1; j <= len; j++) {
         lua_pushnumber(L,j);
         lua_gettable(L,-2);
        mp_encode_lua_type(L,buf,level+1);
    }
}
