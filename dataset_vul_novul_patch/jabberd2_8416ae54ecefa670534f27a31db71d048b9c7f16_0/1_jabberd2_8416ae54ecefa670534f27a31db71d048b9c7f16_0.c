static void _sx_sasl_client_process(sx_t s, sx_plugin_t p, Gsasl_session *sd, const char *mech, const char *in, int inlen) {
    _sx_sasl_t ctx = (_sx_sasl_t) p->private;
    _sx_sasl_sess_t sctx = NULL;
    char *buf = NULL, *out = NULL, *realm = NULL, **ext_id;
    char hostname[256];
    int ret;
#ifdef HAVE_SSL
    int i;
#endif
    size_t buflen, outlen;

    assert(ctx);
    assert(ctx->cb);

     if(mech != NULL) {
         _sx_debug(ZONE, "auth request from client (mechanism=%s)", mech);
 
        if(!gsasl_server_support_p(ctx->gsasl_ctx, mech)) {
              _sx_debug(ZONE, "client requested mechanism (%s) that we didn't offer", mech);
              _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_INVALID_MECHANISM, NULL), 0);
              return;
        }

        /* startup */
        ret = gsasl_server_start(ctx->gsasl_ctx, mech, &sd);
        if(ret != GSASL_OK) {
            _sx_debug(ZONE, "gsasl_server_start failed, no sasl for this conn; (%d): %s", ret, gsasl_strerror(ret));
            _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_TEMPORARY_FAILURE, gsasl_strerror(ret)), 0);
            return;
        }

        /* get the realm */
        (ctx->cb)(sx_sasl_cb_GET_REALM, NULL, (void **) &realm, s, ctx->cbarg);

        /* cleanup any existing session context */
        sctx = gsasl_session_hook_get(sd);
        if (sctx != NULL) free(sctx);

        /* allocate and initialize our per session context */
        sctx = (_sx_sasl_sess_t) calloc(1, sizeof(struct _sx_sasl_sess_st));
        sctx->s = s;
        sctx->ctx = ctx;
        gsasl_session_hook_set(sd, (void *) sctx);
        gsasl_property_set(sd, GSASL_SERVICE, ctx->appname);
        gsasl_property_set(sd, GSASL_REALM, realm);

        /* get hostname */
        hostname[0] = '\0';
        gethostname(hostname, 256);
        hostname[255] = '\0';
        gsasl_property_set(sd, GSASL_HOSTNAME, hostname);

        /* get EXTERNAL data from the ssl plugin */
        ext_id = NULL;
#ifdef HAVE_SSL
        for(i = 0; i < s->env->nplugins; i++)
            if(s->env->plugins[i]->magic == SX_SSL_MAGIC && s->plugin_data[s->env->plugins[i]->index] != NULL)
                ext_id = ((_sx_ssl_conn_t) s->plugin_data[s->env->plugins[i]->index])->external_id;
        if (ext_id != NULL) {
            /* if there is, store it for later */
            for (i = 0; i < SX_CONN_EXTERNAL_ID_MAX_COUNT; i++)
                if (ext_id[i] != NULL) {
                    ctx->ext_id[i] = strdup(ext_id[i]);
                } else {
                    ctx->ext_id[i] = NULL;
                    break;
                }
        }
#endif

        _sx_debug(ZONE, "sasl context initialised for %d", s->tag);

        s->plugin_data[p->index] = (void *) sd;

        if(strcmp(mech, "ANONYMOUS") == 0) {
            /*
             * special case for SASL ANONYMOUS: ignore the initial
             * response provided by the client and generate a random
             * authid to use as the jid node for the user, as
             * specified in XEP-0175
             */
            (ctx->cb)(sx_sasl_cb_GEN_AUTHZID, NULL, (void **)&out, s, ctx->cbarg);
            buf = strdup(out);
            buflen = strlen(buf);
        } else if (strstr(in, "<") != NULL && strncmp(in, "=", strstr(in, "<") - in ) == 0) {
            /* XXX The above check is hackish, but `in` is just weird */
            /* This is a special case for SASL External c2s. See XEP-0178 */
            _sx_debug(ZONE, "gsasl auth string is empty");
            buf = strdup("");
            buflen = strlen(buf);
        } else {
            /* decode and process */
            ret = gsasl_base64_from(in, inlen, &buf, &buflen);
            if (ret != GSASL_OK) {
                _sx_debug(ZONE, "gsasl_base64_from failed, no sasl for this conn; (%d): %s", ret, gsasl_strerror(ret));
                _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_INCORRECT_ENCODING, gsasl_strerror(ret)), 0);
                if(buf != NULL) free(buf);
                return;
            }
        }

        ret = gsasl_step(sd, buf, buflen, &out, &outlen);
    }

    else {
        /* decode and process */
        ret = gsasl_base64_from(in, inlen, &buf, &buflen);
        if (ret != GSASL_OK) {
            _sx_debug(ZONE, "gsasl_base64_from failed, no sasl for this conn; (%d): %s", ret, gsasl_strerror(ret));
            _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_INCORRECT_ENCODING, gsasl_strerror(ret)), 0);
            return;
        }

        if(!sd) {
            _sx_debug(ZONE, "response send before auth request enabling mechanism (decoded: %.*s)", buflen, buf);
            _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_MECH_TOO_WEAK, "response send before auth request enabling mechanism"), 0);
            if(buf != NULL) free(buf);
            return;
        }
        _sx_debug(ZONE, "response from client (decoded: %.*s)", buflen, buf);
        ret = gsasl_step(sd, buf, buflen, &out, &outlen);
    }

    if(buf != NULL) free(buf);

    /* auth completed */
    if(ret == GSASL_OK) {
        _sx_debug(ZONE, "sasl handshake completed");

        /* encode the leftover response */
        ret = gsasl_base64_to(out, outlen, &buf, &buflen);
        if (ret == GSASL_OK) {
            /* send success */
            _sx_nad_write(s, _sx_sasl_success(s, buf, buflen), 0);
            free(buf);

            /* set a notify on the success nad buffer */
            ((sx_buf_t) s->wbufq->front->data)->notify = _sx_sasl_notify_success;
            ((sx_buf_t) s->wbufq->front->data)->notify_arg = (void *) p;
        }
        else {
            _sx_debug(ZONE, "gsasl_base64_to failed, no sasl for this conn; (%d): %s", ret, gsasl_strerror(ret));
            _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_INCORRECT_ENCODING, gsasl_strerror(ret)), 0);
            if(buf != NULL) free(buf);
        }

        if(out != NULL) free(out);

        return;
    }

    /* in progress */
    if(ret == GSASL_NEEDS_MORE) {
        _sx_debug(ZONE, "sasl handshake in progress (challenge: %.*s)", outlen, out);

        /* encode the challenge */
        ret = gsasl_base64_to(out, outlen, &buf, &buflen);
        if (ret == GSASL_OK) {
            _sx_nad_write(s, _sx_sasl_challenge(s, buf, buflen), 0);
            free(buf);
        }
        else {
            _sx_debug(ZONE, "gsasl_base64_to failed, no sasl for this conn; (%d): %s", ret, gsasl_strerror(ret));
            _sx_nad_write(s, _sx_sasl_failure(s, _sasl_err_INCORRECT_ENCODING, gsasl_strerror(ret)), 0);
            if(buf != NULL) free(buf);
        }

        if(out != NULL) free(out);

        return;
    }

    if(out != NULL) free(out);

    /* its over */
    _sx_debug(ZONE, "sasl handshake failed; (%d): %s", ret, gsasl_strerror(ret));

    switch (ret) {
        case GSASL_AUTHENTICATION_ERROR:
	case GSASL_NO_ANONYMOUS_TOKEN:
	case GSASL_NO_AUTHID:
	case GSASL_NO_AUTHZID:
	case GSASL_NO_PASSWORD:
	case GSASL_NO_PASSCODE:
	case GSASL_NO_PIN:
	case GSASL_NO_SERVICE:
	case GSASL_NO_HOSTNAME:
            out = _sasl_err_NOT_AUTHORIZED;
            break;
	case GSASL_UNKNOWN_MECHANISM:
	case GSASL_MECHANISM_PARSE_ERROR:
            out = _sasl_err_INVALID_MECHANISM;
            break;
	case GSASL_BASE64_ERROR:
            out = _sasl_err_INCORRECT_ENCODING;
            break;
        default:
            out = _sasl_err_MALFORMED_REQUEST;
    }
    _sx_nad_write(s, _sx_sasl_failure(s, out, gsasl_strerror(ret)), 0);
}
