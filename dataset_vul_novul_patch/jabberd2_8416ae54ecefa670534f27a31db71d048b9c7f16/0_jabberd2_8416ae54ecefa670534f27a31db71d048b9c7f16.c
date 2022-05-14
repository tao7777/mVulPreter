static int _c2s_sx_sasl_callback(int cb, void *arg, void **res, sx_t s, void *cbarg) {
    c2s_t c2s = (c2s_t) cbarg;
    const char *my_realm, *mech;
    sx_sasl_creds_t creds;
    static char buf[3072];
    char mechbuf[256];
    struct jid_st jid;
    jid_static_buf jid_buf;
    int i, r;
    sess_t sess;
    char skey[44];
    host_t host;

    /* init static jid */
    jid_static(&jid,&jid_buf);

    /* retrieve our session */
    assert(s != NULL);
    sprintf(skey, "%d", s->tag);

    /*
     * Retrieve the session, note that depending on the operation,
     * session may be null.
     */
    sess = xhash_get(c2s->sessions, skey);

    switch(cb) {
        case sx_sasl_cb_GET_REALM:

            if(s->req_to == NULL)   /* this shouldn't happen */
                my_realm = "";

            else {
                /* get host for request */
                host = xhash_get(c2s->hosts, s->req_to);
                if(host == NULL) {
                    log_write(c2s->log, LOG_ERR, "SASL callback for non-existing host: %s", s->req_to);
                    *res = (void *)NULL;
                    return sx_sasl_ret_FAIL;
                }

                my_realm = host->realm;
                if(my_realm == NULL)
                    my_realm = s->req_to;
            }

            strncpy(buf, my_realm, 256);
            *res = (void *)buf;

            log_debug(ZONE, "sx sasl callback: get realm: realm is '%s'", buf);
            return sx_sasl_ret_OK;
            break;

        case sx_sasl_cb_GET_PASS:
            assert(sess != NULL);
            creds = (sx_sasl_creds_t) arg;

            log_debug(ZONE, "sx sasl callback: get pass (authnid=%s, realm=%s)", creds->authnid, creds->realm);

            if(sess->host->ar->get_password && (sess->host->ar->get_password)(
                        sess->host->ar, sess, (char *)creds->authnid, (creds->realm != NULL) ? (char *)creds->realm: "", buf) == 0) {
                *res = buf;
                return sx_sasl_ret_OK;
            }

            return sx_sasl_ret_FAIL;

        case sx_sasl_cb_CHECK_PASS:
            assert(sess != NULL);
            creds = (sx_sasl_creds_t) arg;

            log_debug(ZONE, "sx sasl callback: check pass (authnid=%s, realm=%s)", creds->authnid, creds->realm);

            if(sess->host->ar->check_password != NULL) {
                if ((sess->host->ar->check_password)(
                            sess->host->ar, sess, (char *)creds->authnid, (creds->realm != NULL) ? (char *)creds->realm : "", (char *)creds->pass) == 0)
                    return sx_sasl_ret_OK;
                else
                    return sx_sasl_ret_FAIL;
            }

            if(sess->host->ar->get_password != NULL) {
                if ((sess->host->ar->get_password)(sess->host->ar, sess, (char *)creds->authnid, (creds->realm != NULL) ? (char *)creds->realm : "", buf) != 0)
                    return sx_sasl_ret_FAIL;

                if (strcmp(creds->pass, buf)==0)
                    return sx_sasl_ret_OK;
            }

            return sx_sasl_ret_FAIL;
            break;

        case sx_sasl_cb_CHECK_AUTHZID:
            assert(sess != NULL);
            creds = (sx_sasl_creds_t) arg;

            /* we need authzid to validate */
            if(creds->authzid == NULL || creds->authzid[0] == '\0')
                return sx_sasl_ret_FAIL;

            /* authzid must be a valid jid */
            if(jid_reset(&jid, creds->authzid, -1) == NULL)
                return sx_sasl_ret_FAIL;

            /* and have domain == stream to addr */
            if(!s->req_to || (strcmp(jid.domain, s->req_to) != 0))
                return sx_sasl_ret_FAIL;

            /* and have no resource */
            if(jid.resource[0] != '\0')
                return sx_sasl_ret_FAIL;

            /* and user has right to authorize as */
            if (sess->host->ar->user_authz_allowed) {
                if (sess->host->ar->user_authz_allowed(sess->host->ar, sess, (char *)creds->authnid, (char *)creds->realm, (char *)creds->authzid))
                        return sx_sasl_ret_OK;
            } else {
                if (strcmp(creds->authnid, jid.node) == 0 &&
                    (sess->host->ar->user_exists)(sess->host->ar, sess, jid.node, jid.domain))
                    return sx_sasl_ret_OK;
            }

            return sx_sasl_ret_FAIL;

        case sx_sasl_cb_GEN_AUTHZID:
            /* generate a jid for SASL ANONYMOUS */
            jid_reset(&jid, s->req_to, -1);

            /* make node a random string */
            jid_random_part(&jid, jid_NODE);

            strcpy(buf, jid.node);

            *res = (void *)buf;

            return sx_sasl_ret_OK;
            break;

        case sx_sasl_cb_CHECK_MECH:
            mech = (char *)arg;

            strncpy(mechbuf, mech, sizeof(mechbuf));
             mechbuf[sizeof(mechbuf)-1]='\0';
             for(i = 0; mechbuf[i]; i++) mechbuf[i] = tolower(mechbuf[i]);
 
            log_debug(ZONE, "sx sasl callback: check mech (mech=%s)", mechbuf);

             /* get host for request */
             host = xhash_get(c2s->hosts, s->req_to);
             if(host == NULL) {
                log_write(c2s->log, LOG_WARNING, "SASL callback for non-existing host: %s", s->req_to);
                return sx_sasl_ret_FAIL;
            }

            /* Determine if our configuration will let us use this mechanism.
             * We support different mechanisms for both SSL and normal use */
            if (strcmp(mechbuf, "digest-md5") == 0) {
                /* digest-md5 requires that our authreg support get_password */
                if (host->ar->get_password == NULL)
                    return sx_sasl_ret_FAIL;
            } else if (strcmp(mechbuf, "plain") == 0) {
                /* plain requires either get_password or check_password */
                if (host->ar->get_password == NULL && host->ar->check_password == NULL)
                    return sx_sasl_ret_FAIL;
            }

            /* Using SSF is potentially dangerous, as SASL can also set the
             * SSF of the connection. However, SASL shouldn't do so until after
             * we've finished mechanism establishment
             */
            if (s->ssf>0) {
                r = snprintf(buf, sizeof(buf), "authreg.ssl-mechanisms.sasl.%s",mechbuf);
                if (r < -1 || r > sizeof(buf))
                    return sx_sasl_ret_FAIL;
                if(config_get(c2s->config,buf) != NULL)
                    return sx_sasl_ret_OK;
            }

            r = snprintf(buf, sizeof(buf), "authreg.mechanisms.sasl.%s",mechbuf);
            if (r < -1 || r > sizeof(buf))
                return sx_sasl_ret_FAIL;

            /* Work out if our configuration will let us use this mechanism */
            if(config_get(c2s->config,buf) != NULL)
                return sx_sasl_ret_OK;
            else
                return sx_sasl_ret_FAIL;
        default:
            break;
    }

    return sx_sasl_ret_FAIL;
}
