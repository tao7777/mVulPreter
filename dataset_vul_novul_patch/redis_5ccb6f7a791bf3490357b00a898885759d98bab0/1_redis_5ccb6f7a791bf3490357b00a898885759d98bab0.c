void mp_decode_to_lua_array(lua_State *L, mp_cur *c, size_t len) {
    assert(len <= UINT_MAX);
     int index = 1;
 
     lua_newtable(L);
     while(len--) {
         lua_pushnumber(L,index++);
         mp_decode_to_lua_type(L,c);
        if (c->err) return;
        lua_settable(L,-3);
    }
}
