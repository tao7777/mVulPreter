static int read_request_line(request_rec *r, apr_bucket_brigade *bb)
{
    const char *ll;
    const char *uri;
    const char *pro;

    unsigned int major = 1, minor = 0;   /* Assume HTTP/1.0 if non-"HTTP" protocol */
    char http[5];
    apr_size_t len;
    int num_blank_lines = 0;
    int max_blank_lines = r->server->limit_req_fields;
    core_server_config *conf = ap_get_core_module_config(r->server->module_config);
    int strict = conf->http_conformance & AP_HTTP_CONFORMANCE_STRICT;
    int enforce_strict = !(conf->http_conformance & AP_HTTP_CONFORMANCE_LOGONLY);

    if (max_blank_lines <= 0) {
        max_blank_lines = DEFAULT_LIMIT_REQUEST_FIELDS;
    }

    /* Read past empty lines until we get a real request line,
     * a read error, the connection closes (EOF), or we timeout.
     *
     * We skip empty lines because browsers have to tack a CRLF on to the end
     * of POSTs to support old CERN webservers.  But note that we may not
     * have flushed any previous response completely to the client yet.
     * We delay the flush as long as possible so that we can improve
     * performance for clients that are pipelining requests.  If a request
     * is pipelined then we won't block during the (implicit) read() below.
     * If the requests aren't pipelined, then the client is still waiting
     * for the final buffer flush from us, and we will block in the implicit
     * read().  B_SAFEREAD ensures that the BUFF layer flushes if it will
     * have to block during a read.
     */

    do {
        apr_status_t rv;

        /* ensure ap_rgetline allocates memory each time thru the loop
         * if there are empty lines
         */
        r->the_request = NULL;
        rv = ap_rgetline(&(r->the_request), (apr_size_t)(r->server->limit_req_line + 2),
                         &len, r, 0, bb);

        if (rv != APR_SUCCESS) {
            r->request_time = apr_time_now();

            /* ap_rgetline returns APR_ENOSPC if it fills up the
             * buffer before finding the end-of-line.  This is only going to
             * happen if it exceeds the configured limit for a request-line.
              */
             if (APR_STATUS_IS_ENOSPC(rv)) {
                 r->status    = HTTP_REQUEST_URI_TOO_LARGE;
                r->proto_num = HTTP_VERSION(1,0);
                r->protocol  = apr_pstrdup(r->pool, "HTTP/1.0");
             }
             else if (APR_STATUS_IS_TIMEUP(rv)) {
                 r->status = HTTP_REQUEST_TIME_OUT;
             }
             else if (APR_STATUS_IS_EINVAL(rv)) {
                 r->status = HTTP_BAD_REQUEST;
             }
             return 0;
         }
     } while ((len <= 0) && (++num_blank_lines < max_blank_lines));

    if (APLOGrtrace5(r)) {
        ap_log_rerror(APLOG_MARK, APLOG_TRACE5, 0, r,
                      "Request received from client: %s",
                      ap_escape_logitem(r->pool, r->the_request));
    }

    r->request_time = apr_time_now();
    ll = r->the_request;
    r->method = ap_getword_white(r->pool, &ll);

    uri = ap_getword_white(r->pool, &ll);

    /* Provide quick information about the request method as soon as known */

    r->method_number = ap_method_number_of(r->method);
    if (r->method_number == M_GET && r->method[0] == 'H') {
        r->header_only = 1;
    }

    ap_parse_uri(r, uri);

    if (ll[0]) {
        r->assbackwards = 0;
        pro = ll;
        len = strlen(ll);
    } else {
        r->assbackwards = 1;
        pro = "HTTP/0.9";
        len = 8;
        if (conf->http09_enable == AP_HTTP09_DISABLE) {
                r->status = HTTP_VERSION_NOT_SUPPORTED;
                r->protocol = apr_pstrmemdup(r->pool, pro, len);
                /* If we deny 0.9, send error message with 1.x */
                r->assbackwards = 0;
                r->proto_num = HTTP_VERSION(0, 9);
                r->connection->keepalive = AP_CONN_CLOSE;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02401)
                              "HTTP/0.9 denied by server configuration");
                return 0;
        }
    }
    r->protocol = apr_pstrmemdup(r->pool, pro, len);

    /* Avoid sscanf in the common case */
    if (len == 8
        && pro[0] == 'H' && pro[1] == 'T' && pro[2] == 'T' && pro[3] == 'P'
        && pro[4] == '/' && apr_isdigit(pro[5]) && pro[6] == '.'
        && apr_isdigit(pro[7])) {
        r->proto_num = HTTP_VERSION(pro[5] - '0', pro[7] - '0');
    }
    else {
        if (strict) {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02418)
                          "Invalid protocol '%s'", r->protocol);
            if (enforce_strict) {
                r->status = HTTP_BAD_REQUEST;
                return 0;
            }
        }
        if (3 == sscanf(r->protocol, "%4s/%u.%u", http, &major, &minor)
            && (strcasecmp("http", http) == 0)
            && (minor < HTTP_VERSION(1, 0)) ) { /* don't allow HTTP/0.1000 */
            r->proto_num = HTTP_VERSION(major, minor);
        }
        else {
            r->proto_num = HTTP_VERSION(1, 0);
        }
    }

    if (strict) {
        int err = 0;
        if (ap_has_cntrl(r->the_request)) {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02420)
                          "Request line must not contain control characters");
            err = HTTP_BAD_REQUEST;
        }
        if (r->parsed_uri.fragment) {
            /* RFC3986 3.5: no fragment */
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02421)
                          "URI must not contain a fragment");
            err = HTTP_BAD_REQUEST;
        }
        else if (r->parsed_uri.user || r->parsed_uri.password) {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02422)
                          "URI must not contain a username/password");
            err = HTTP_BAD_REQUEST;
        }
        else if (r->method_number == M_INVALID) {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02423)
                          "Invalid HTTP method string: %s", r->method);
            err = HTTP_NOT_IMPLEMENTED;
        }
        else if (r->assbackwards == 0 && r->proto_num < HTTP_VERSION(1, 0)) {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02424)
                          "HTTP/0.x does not take a protocol");
            err = HTTP_BAD_REQUEST;
        }

        if (err && enforce_strict) {
            r->status = err;
            return 0;
        }
    }

    return 1;
}
