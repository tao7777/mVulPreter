int _mkp_stage_30(struct plugin *p,
        struct client_session *cs,
        struct session_request *sr)
{
    mk_ptr_t referer;
    (void) p;
     (void) cs;
 
     PLUGIN_TRACE("[FD %i] Mandril validating URL", cs->socket);

    if (mk_security_check_url(sr->uri_processed) < 0) {
         PLUGIN_TRACE("[FD %i] Close connection, blocked URL", cs->socket);
         mk_api->header_set_http_status(sr, MK_CLIENT_FORBIDDEN);
         return MK_PLUGIN_RET_CLOSE_CONX;
    }

    PLUGIN_TRACE("[FD %d] Mandril validating hotlinking", cs->socket);
    referer = mk_api->header_get(&sr->headers_toc, "Referer", strlen("Referer"));
    if (mk_security_check_hotlink(sr->uri_processed, sr->host, referer) < 0) {
        PLUGIN_TRACE("[FD %i] Close connection, deny hotlinking.", cs->socket);
        mk_api->header_set_http_status(sr, MK_CLIENT_FORBIDDEN);
        return MK_PLUGIN_RET_CLOSE_CONX;
    }

    return MK_PLUGIN_RET_NOT_ME;
}
