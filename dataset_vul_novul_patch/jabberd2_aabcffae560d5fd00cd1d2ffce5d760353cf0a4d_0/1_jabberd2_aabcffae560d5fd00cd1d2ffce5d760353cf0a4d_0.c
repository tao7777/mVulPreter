static void _out_verify(conn_t out, nad_t nad) {
    int attr, ns;
    jid_t from, to;
    conn_t in;
    char *rkey;
    int valid;

    attr = nad_find_attr(nad, 0, -1, "from", NULL);
    if(attr < 0 || (from = jid_new(NAD_AVAL(nad, attr), NAD_AVAL_L(nad, attr))) == NULL) {
        log_debug(ZONE, "missing or invalid from on db verify packet");
        nad_free(nad);
        return;
    }

    attr = nad_find_attr(nad, 0, -1, "to", NULL);
    if(attr < 0 || (to = jid_new(NAD_AVAL(nad, attr), NAD_AVAL_L(nad, attr))) == NULL) {
        log_debug(ZONE, "missing or invalid to on db verify packet");
        jid_free(from);
        nad_free(nad);
        return;
    }

    attr = nad_find_attr(nad, 0, -1, "id", NULL);
    if(attr < 0) {
        log_debug(ZONE, "missing id on db verify packet");
        jid_free(from);
        jid_free(to);
        nad_free(nad);
        return;
    }

    /* get the incoming conn */
    in = xhash_getx(out->s2s->in, NAD_AVAL(nad, attr), NAD_AVAL_L(nad, attr));
    if(in == NULL) {
        log_debug(ZONE, "got a verify for incoming conn %.*s, but it doesn't exist, dropping the packet", NAD_AVAL_L(nad, attr), NAD_AVAL(nad, attr));
        jid_free(from);
        jid_free(to);
        nad_free(nad);
        return;
    }

     rkey = s2s_route_key(NULL, to->domain, from->domain);
 
     attr = nad_find_attr(nad, 0, -1, "type", "valid");
    if(attr >= 0) {
         xhash_put(in->states, pstrdup(xhash_pool(in->states), rkey), (void *) conn_VALID);
         log_write(in->s2s->log, LOG_NOTICE, "[%d] [%s, port=%d] incoming route '%s' is now valid%s%s", in->fd->fd, in->ip, in->port, rkey, (in->s->flags & SX_SSL_WRAPPER) ? ", TLS negotiated" : "", in->s->compressed ? ", ZLIB compression enabled" : "");
         valid = 1;
    } else {
        log_write(in->s2s->log, LOG_NOTICE, "[%d] [%s, port=%d] incoming route '%s' is now invalid", in->fd->fd, in->ip, in->port, rkey);
        valid = 0;
    }

    free(rkey);

    nad_free(nad);

    /* decrement outstanding verify counter */
    --out->verify;

    /* let them know what happened */
    nad = nad_new();

    ns = nad_add_namespace(nad, uri_DIALBACK, "db");
    nad_append_elem(nad, ns, "result", 0);
    nad_append_attr(nad, -1, "to", from->domain);
    nad_append_attr(nad, -1, "from", to->domain);
    nad_append_attr(nad, -1, "type", valid ? "valid" : "invalid");

    /* off it goes */
    sx_nad_write(in->s, nad);

    /* if invalid, close the stream */
    if (!valid) {
        /* generate stream error */
        sx_error(in->s, stream_err_INVALID_ID, "dialback negotiation failed");

        /* close the incoming stream */
        sx_close(in->s);
    }

    jid_free(from);
    jid_free(to);
}
