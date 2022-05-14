 apr_status_t ap_http_filter(ap_filter_t *f, apr_bucket_brigade *b,
                            ap_input_mode_t mode, apr_read_type_e block,
                            apr_off_t readbytes)
 {
     core_server_config *conf;
     apr_bucket *e;
    http_ctx_t *ctx = f->ctx;
    apr_status_t rv;
    apr_off_t totalread;
    int again;

    conf = (core_server_config *)
        ap_get_module_config(f->r->server->module_config, &core_module);

    /* just get out of the way of things we don't want. */
    if (mode != AP_MODE_READBYTES && mode != AP_MODE_GETLINE) {
        return ap_get_brigade(f->next, b, mode, block, readbytes);
    }

    if (!ctx) {
        const char *tenc, *lenp;
        f->ctx = ctx = apr_pcalloc(f->r->pool, sizeof(*ctx));
        ctx->state = BODY_NONE;

        /* LimitRequestBody does not apply to proxied responses.
         * Consider implementing this check in its own filter.
         * Would adding a directive to limit the size of proxied
         * responses be useful?
         */
        if (!f->r->proxyreq) {
            ctx->limit = ap_get_limit_req_body(f->r);
        }
        else {
            ctx->limit = 0;
        }

        tenc = apr_table_get(f->r->headers_in, "Transfer-Encoding");
        lenp = apr_table_get(f->r->headers_in, "Content-Length");

        if (tenc) {
            if (strcasecmp(tenc, "chunked") == 0 /* fast path */
                    || ap_find_last_token(f->r->pool, tenc, "chunked")) {
                ctx->state = BODY_CHUNK;
            }
            else if (f->r->proxyreq == PROXYREQ_RESPONSE) {
                /* http://tools.ietf.org/html/draft-ietf-httpbis-p1-messaging-23
                 * Section 3.3.3.3: "If a Transfer-Encoding header field is
                 * present in a response and the chunked transfer coding is not
                 * the final encoding, the message body length is determined by
                  * reading the connection until it is closed by the server."
                  */
                 ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, f->r, APLOGNO(02555)
                              "Unknown Transfer-Encoding: %s; "
                              "using read-until-close", tenc);
                 tenc = NULL;
             }
             else {
                /* Something that isn't a HTTP request, unless some future
                 * edition defines new transfer encodings, is unsupported.
                 */
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, f->r, APLOGNO(01585)
                              "Unknown Transfer-Encoding: %s", tenc);
                return APR_EGENERAL;
            }
            lenp = NULL;
        }
        if (lenp) {
            char *endstr;

            ctx->state = BODY_LENGTH;

            /* Protects against over/underflow, non-digit chars in the
             * string (excluding leading space) (the endstr checks)
             * and a negative number. */
            if (apr_strtoff(&ctx->remaining, lenp, &endstr, 10)
                     || endstr == lenp || *endstr || ctx->remaining < 0) {
 
                 ctx->remaining = 0;
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, f->r, APLOGNO(01587)
                              "Invalid Content-Length");
 
                return APR_EINVAL;
             }
 
             /* If we have a limit in effect and we know the C-L ahead of
              * time, stop it here if it is invalid.
              */
             if (ctx->limit && ctx->limit < ctx->remaining) {
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, f->r, APLOGNO(01588)
                          "Requested content-length of %" APR_OFF_T_FMT
                          " is larger than the configured limit"
                          " of %" APR_OFF_T_FMT, ctx->remaining, ctx->limit);
                 return APR_ENOSPC;
             }
         }

        /* If we don't have a request entity indicated by the headers, EOS.
         * (BODY_NONE is a valid intermediate state due to trailers,
         *  but it isn't a valid starting state.)
         *
         * RFC 2616 Section 4.4 note 5 states that connection-close
         * is invalid for a request entity - request bodies must be
         * denoted by C-L or T-E: chunked.
         *
         * Note that since the proxy uses this filter to handle the
         * proxied *response*, proxy responses MUST be exempt.
         */
        if (ctx->state == BODY_NONE && f->r->proxyreq != PROXYREQ_RESPONSE) {
            e = apr_bucket_eos_create(f->c->bucket_alloc);
            APR_BRIGADE_INSERT_TAIL(b, e);
            ctx->eos_sent = 1;
            return APR_SUCCESS;
        }

        /* Since we're about to read data, send 100-Continue if needed.
         * Only valid on chunked and C-L bodies where the C-L is > 0. */
        if ((ctx->state == BODY_CHUNK
                || (ctx->state == BODY_LENGTH && ctx->remaining > 0))
                && f->r->expecting_100 && f->r->proto_num >= HTTP_VERSION(1,1)
                && !(f->r->eos_sent || f->r->bytes_sent)) {
            if (!ap_is_HTTP_SUCCESS(f->r->status)) {
                ctx->state = BODY_NONE;
                ctx->eos_sent = 1;
            }
            else {
                char *tmp;
                int len;
                apr_bucket_brigade *bb;

                bb = apr_brigade_create(f->r->pool, f->c->bucket_alloc);

                /* if we send an interim response, we're no longer
                 * in a state of expecting one.
                 */
                f->r->expecting_100 = 0;
                tmp = apr_pstrcat(f->r->pool, AP_SERVER_PROTOCOL " ",
                        ap_get_status_line(HTTP_CONTINUE), CRLF CRLF, NULL);
                len = strlen(tmp);
                ap_xlate_proto_to_ascii(tmp, len);
                e = apr_bucket_pool_create(tmp, len, f->r->pool,
                        f->c->bucket_alloc);
                APR_BRIGADE_INSERT_HEAD(bb, e);
                e = apr_bucket_flush_create(f->c->bucket_alloc);
                 APR_BRIGADE_INSERT_TAIL(bb, e);
 
                 rv = ap_pass_brigade(f->c->output_filters, bb);
                apr_brigade_cleanup(bb);
                 if (rv != APR_SUCCESS) {
                     return AP_FILTER_ERROR;
                 }
            }
        }
    }

    /* sanity check in case we're read twice */
    if (ctx->eos_sent) {
        e = apr_bucket_eos_create(f->c->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(b, e);
        return APR_SUCCESS;
    }

    do {
        apr_brigade_cleanup(b);
        again = 0; /* until further notice */

        /* read and handle the brigade */
        switch (ctx->state) {
         case BODY_CHUNK:
         case BODY_CHUNK_PART:
         case BODY_CHUNK_EXT:
        case BODY_CHUNK_LF:
        case BODY_CHUNK_END:
        case BODY_CHUNK_END_LF: {
 
             rv = ap_get_brigade(f->next, b, AP_MODE_GETLINE, block, 0);
 
            /* for timeout */
            if (block == APR_NONBLOCK_READ
                    && ((rv == APR_SUCCESS && APR_BRIGADE_EMPTY(b))
                            || (APR_STATUS_IS_EAGAIN(rv)))) {
                return APR_EAGAIN;
            }

            if (rv == APR_EOF) {
                return APR_INCOMPLETE;
            }

            if (rv != APR_SUCCESS) {
                return rv;
            }

            e = APR_BRIGADE_FIRST(b);
            while (e != APR_BRIGADE_SENTINEL(b)) {
                const char *buffer;
                apr_size_t len;

                if (!APR_BUCKET_IS_METADATA(e)) {
                    rv = apr_bucket_read(e, &buffer, &len, APR_BLOCK_READ);

                    if (rv == APR_SUCCESS) {
                        rv = parse_chunk_size(ctx, buffer, len,
                                 f->r->server->limit_req_fieldsize);
                     }
                     if (rv != APR_SUCCESS) {
                        ap_log_rerror(APLOG_MARK, APLOG_INFO, rv, f->r, APLOGNO(01590)
                                      "Error reading chunk %s ",
                                      (APR_ENOSPC == rv) ? "(overflow)" : "");
                         return rv;
                     }
                 }

                apr_bucket_delete(e);
                e = APR_BRIGADE_FIRST(b);
            }
            again = 1; /* come around again */
 
             if (ctx->state == BODY_CHUNK_TRAILER) {
                 /* Treat UNSET as DISABLE - trailers aren't merged by default */
                return read_chunked_trailers(ctx, f, b,
                            conf->merge_trailers == AP_MERGE_TRAILERS_ENABLE);
             }
 
             break;
        }
        case BODY_NONE:
        case BODY_LENGTH:
        case BODY_CHUNK_DATA: {

            /* Ensure that the caller can not go over our boundary point. */
            if (ctx->state != BODY_NONE && ctx->remaining < readbytes) {
                readbytes = ctx->remaining;
            }
            if (readbytes > 0) {

                rv = ap_get_brigade(f->next, b, mode, block, readbytes);

                /* for timeout */
                if (block == APR_NONBLOCK_READ
                        && ((rv == APR_SUCCESS && APR_BRIGADE_EMPTY(b))
                                || (APR_STATUS_IS_EAGAIN(rv)))) {
                    return APR_EAGAIN;
                }

                if (rv == APR_EOF && ctx->state != BODY_NONE
                        && ctx->remaining > 0) {
                    return APR_INCOMPLETE;
                }

                if (rv != APR_SUCCESS) {
                    return rv;
                }

                /* How many bytes did we just read? */
                apr_brigade_length(b, 0, &totalread);

                /* If this happens, we have a bucket of unknown length.  Die because
                 * it means our assumptions have changed. */
                AP_DEBUG_ASSERT(totalread >= 0);

                if (ctx->state != BODY_NONE) {
                    ctx->remaining -= totalread;
                    if (ctx->remaining > 0) {
                        e = APR_BRIGADE_LAST(b);
                        if (APR_BUCKET_IS_EOS(e)) {
                            apr_bucket_delete(e);
                            return APR_INCOMPLETE;
                        }
                    }
                    else if (ctx->state == BODY_CHUNK_DATA) {
                        /* next chunk please */
                        ctx->state = BODY_CHUNK_END;
                        ctx->chunk_used = 0;
                    }
                }

            }

            /* If we have no more bytes remaining on a C-L request,
             * save the caller a round trip to discover EOS.
             */
            if (ctx->state == BODY_LENGTH && ctx->remaining == 0) {
                e = apr_bucket_eos_create(f->c->bucket_alloc);
                APR_BRIGADE_INSERT_TAIL(b, e);
                ctx->eos_sent = 1;
            }

            /* We have a limit in effect. */
            if (ctx->limit) {
                /* FIXME: Note that we might get slightly confused on chunked inputs
                 * as we'd need to compensate for the chunk lengths which may not
                  * really count.  This seems to be up for interpretation.  */
                 ctx->limit_used += totalread;
                 if (ctx->limit < ctx->limit_used) {
                    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, f->r, APLOGNO(01591)
                                  "Read content-length of %" APR_OFF_T_FMT
                                  " is larger than the configured limit"
                                  " of %" APR_OFF_T_FMT, ctx->limit_used, ctx->limit);
                     return APR_ENOSPC;
                 }
             }

            break;
        }
        case BODY_CHUNK_TRAILER: {

            rv = ap_get_brigade(f->next, b, mode, block, readbytes);

            /* for timeout */
            if (block == APR_NONBLOCK_READ
                    && ((rv == APR_SUCCESS && APR_BRIGADE_EMPTY(b))
                            || (APR_STATUS_IS_EAGAIN(rv)))) {
                return APR_EAGAIN;
            }

            if (rv != APR_SUCCESS) {
                return rv;
            }

             break;
         }
         default: {
            /* Should not happen */
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r, APLOGNO(02901)
                          "Unexpected body state (%i)", (int)ctx->state);
            return APR_EGENERAL;
         }
         }
 
    } while (again);

    return APR_SUCCESS;
}
