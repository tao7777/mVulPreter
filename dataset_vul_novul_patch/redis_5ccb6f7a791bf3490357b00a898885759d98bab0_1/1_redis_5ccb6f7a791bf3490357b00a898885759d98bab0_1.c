void mp_encode_lua_table_as_map(lua_State *L, mp_buf *buf, int level) {
    size_t len = 0;

    /* First step: count keys into table. No other way to do it with the
      * Lua API, we need to iterate a first time. Note that an alternative
      * would be to do a single run, and then hack the buffer to insert the
      * map opcodes for message pack. Too hackish for this lib. */
     lua_pushnil(L);
     while(lua_next(L,-2)) {
         lua_pop(L,1); /* remove value, keep key for next iteration. */
        len++;
    }

    /* Step two: actually encoding of the map. */
    mp_encode_map(L,buf,len);
    lua_pushnil(L);
    while(lua_next(L,-2)) {
        /* Stack: ... key value */
        lua_pushvalue(L,-2); /* Stack: ... key value key */
        mp_encode_lua_type(L,buf,level+1); /* encode key */
        mp_encode_lua_type(L,buf,level+1); /* encode val */
    }
}
