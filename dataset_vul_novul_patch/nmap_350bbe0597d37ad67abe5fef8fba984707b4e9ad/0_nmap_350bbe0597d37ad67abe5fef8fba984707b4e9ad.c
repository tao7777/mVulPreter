static int do_session_handshake (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    struct ssh_userdata *sshu = NULL;

    assert(lua_gettop(L) == 4);
    sshu = (struct ssh_userdata *) nseU_checkudata(L, 3, SSH2_UDATA, "ssh2");

    while ((rc = libssh2_session_handshake(sshu->session, sshu->sp[0])) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 3, "filter");
        lua_pushvalue(L, 3);

        assert(lua_status(L) == LUA_OK);
        lua_callk(L, 1, 0, 0, do_session_handshake);
    }
 
     if (rc) {
         libssh2_session_free(sshu->session);
        sshu->session = NULL;
         return luaL_error(L, "Unable to complete libssh2 handshake.");
     }
 
    lua_settop(L, 3);

    return 1;
}
