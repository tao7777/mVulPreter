static int on_http_message_complete(http_parser* parser)
{
    struct clt_info *info = parser->data;
    ws_svr *svr = ws_svr_from_ses(info->ses);
    info->request->version_major = parser->http_major;
    info->request->version_minor = parser->http_minor;
    info->request->method = parser->method;

    dict_entry *entry;
    dict_iterator *iter = dict_get_iterator(info->request->headers);
    while ((entry = dict_next(iter)) != NULL) {
        log_trace("Header: %s: %s", (char *)entry->key, (char *)entry->val);
    }
    dict_release_iterator(iter);

    if (info->request->method != HTTP_GET)
        goto error;
    if (http_request_get_header(info->request, "Host") == NULL)
        goto error;
    double version = info->request->version_major + info->request->version_minor * 0.1;
    if (version < 1.1)
        goto error;
    const char *upgrade = http_request_get_header(info->request, "Upgrade");
     if (upgrade == NULL || strcasecmp(upgrade, "websocket") != 0)
         goto error;
     const char *connection = http_request_get_header(info->request, "Connection");
    if (connection == NULL || strlen(connection) > UT_WS_SVR_MAX_HEADER_SIZE)
         goto error;
     else {
         bool found_upgrade = false;
        int count;
        sds *tokens = sdssplitlen(connection, strlen(connection), ",", 1, &count); 
        if (tokens == NULL)
            goto error;
        for (int i = 0; i < count; i++) {
            sds token = tokens[i];
            sdstrim(token, " ");
            if (strcasecmp(token, "Upgrade") == 0) {
                found_upgrade = true;
                break;
            }
        }
        sdsfreesplitres(tokens, count);
        if (!found_upgrade)
            goto error;
    }
    const char *ws_version = http_request_get_header(info->request, "Sec-WebSocket-Version");
    if (ws_version == NULL || strcmp(ws_version, "13") != 0)
        goto error;
    const char *ws_key = http_request_get_header(info->request, "Sec-WebSocket-Key");
    if (ws_key == NULL)
        goto error;
    const char *protocol_list = http_request_get_header(info->request, "Sec-WebSocket-Protocol");
    if (protocol_list && !is_good_protocol(protocol_list, svr->protocol))
        goto error;
    if (strlen(svr->origin) > 0) {
        const char *origin = http_request_get_header(info->request, "Origin");
        if (origin == NULL || !is_good_origin(origin, svr->origin))
            goto error;
    }

    if (svr->type.on_privdata_alloc) {
        info->privdata = svr->type.on_privdata_alloc(svr);
        if (info->privdata == NULL)
            goto error;
    }
    info->upgrade = true;
    info->remote = sdsnew(http_get_remote_ip(info->ses, info->request));
    info->url = sdsnew(info->request->url);
    if (svr->type.on_upgrade) {
        svr->type.on_upgrade(info->ses, info->remote);
    }
    if (protocol_list) {
        send_hand_shake_reply(info->ses, svr->protocol, ws_key);
    } else {
        send_hand_shake_reply(info->ses, NULL, ws_key);
    }

    return 0;

error:
    ws_svr_close_clt(ws_svr_from_ses(info->ses), info->ses);
    return -1;
}
