int mp_pack(lua_State *L) {
    int nargs = lua_gettop(L);
    int i;
    mp_buf *buf;

     if (nargs == 0)
         return luaL_argerror(L, 0, "MessagePack pack needs input.");
 
    if (!lua_checkstack(L, nargs))
        return luaL_argerror(L, 0, "Too many arguments for MessagePack pack.");

     buf = mp_buf_new(L);
     for(i = 1; i <= nargs; i++) {
         /* Copy argument i to top of stack for _encode processing;
         * the encode function pops it from the stack when complete. */
        lua_pushvalue(L, i);

        mp_encode_lua_type(L,buf,0);

        lua_pushlstring(L,(char*)buf->b,buf->len);

        /* Reuse the buffer for the next operation by
         * setting its free count to the total buffer size
         * and the current position to zero. */
        buf->free += buf->len;
        buf->len = 0;
    }
    mp_buf_free(L, buf);

    /* Concatenate all nargs buffers together */
    lua_concat(L, nargs);
    return 1;
}
