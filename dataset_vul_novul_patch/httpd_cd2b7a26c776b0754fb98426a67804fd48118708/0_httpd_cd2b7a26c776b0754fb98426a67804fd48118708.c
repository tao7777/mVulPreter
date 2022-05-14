AP_DECLARE(int) ap_process_request_internal(request_rec *r)
{
    int file_req = (r->main && r->filename);
    int access_status;
    core_dir_config *d;

    /* Ignore embedded %2F's in path for proxy requests */
    if (!r->proxyreq && r->parsed_uri.path) {
        d = ap_get_core_module_config(r->per_dir_config);
        if (d->allow_encoded_slashes) {
            access_status = ap_unescape_url_keep2f(r->parsed_uri.path, d->decode_encoded_slashes);
        }
        else {
            access_status = ap_unescape_url(r->parsed_uri.path);
        }
        if (access_status) {
            if (access_status == HTTP_NOT_FOUND) {
                if (! d->allow_encoded_slashes) {
                    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(00026)
                                  "found %%2f (encoded '/') in URI "
                                  "(decoded='%s'), returning 404",
                                  r->parsed_uri.path);
                }
            }
            return access_status;
        }
    }

    ap_getparents(r->uri);     /* OK --- shrinking transformations... */

    /* All file subrequests are a huge pain... they cannot bubble through the
     * next several steps.  Only file subrequests are allowed an empty uri,
     * otherwise let translate_name kill the request.
     */
    if (!file_req) {
        if ((access_status = ap_location_walk(r))) {
            return access_status;
        }
        if ((access_status = ap_if_walk(r))) {
            return access_status;
        }

        /* Don't set per-dir loglevel if LogLevelOverride is set */
        if (!r->connection->log) {
            d = ap_get_core_module_config(r->per_dir_config);
            if (d->log)
                r->log = d->log;
        }

        if ((access_status = ap_run_translate_name(r))) {
            return decl_die(access_status, "translate", r);
        }
    }

    /* Reset to the server default config prior to running map_to_storage
     */
    r->per_dir_config = r->server->lookup_defaults;

    if ((access_status = ap_run_map_to_storage(r))) {
        /* This request wasn't in storage (e.g. TRACE) */
        return access_status;
    }

    /* Rerun the location walk, which overrides any map_to_storage config.
     */
    if ((access_status = ap_location_walk(r))) {
        return access_status;
    }
    if ((access_status = ap_if_walk(r))) {
        return access_status;
    }

    /* Don't set per-dir loglevel if LogLevelOverride is set */
    if (!r->connection->log) {
        d = ap_get_core_module_config(r->per_dir_config);
        if (d->log)
            r->log = d->log;
    }

    if ((access_status = ap_run_post_perdir_config(r))) {
        return access_status;
    }

    /* Only on the main request! */
    if (r->main == NULL) {
        if ((access_status = ap_run_header_parser(r))) {
            return access_status;
        }
    }

    /* Skip authn/authz if the parent or prior request passed the authn/authz,
     * and that configuration didn't change (this requires optimized _walk()
     * functions in map_to_storage that use the same merge results given
     * identical input.)  If the config changes, we must re-auth.
     */
    if (r->prev && (r->prev->per_dir_config == r->per_dir_config)) {
        r->user = r->prev->user;
        r->ap_auth_type = r->prev->ap_auth_type;
    }
    else if (r->main && (r->main->per_dir_config == r->per_dir_config)) {
        r->user = r->main->user;
        r->ap_auth_type = r->main->ap_auth_type;
    }
    else {
        switch (ap_satisfies(r)) {
        case SATISFY_ALL:
        case SATISFY_NOSPEC:
            if ((access_status = ap_run_access_checker(r)) != OK) {
                return decl_die(access_status,
                                "check access (with Satisfy All)", r);
             }
 
             access_status = ap_run_access_checker_ex(r);
            if (access_status == DECLINED
                || (access_status == OK && ap_run_force_authn(r) == OK)) {
                 if ((access_status = ap_run_check_user_id(r)) != OK) {
                     return decl_die(access_status, "check user", r);
                 }
                if (r->user == NULL) {
                    /* don't let buggy authn module crash us in authz */
                    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00027)
                                  "No authentication done but request not "
                                  "allowed without authentication for %s. "
                                  "Authentication not configured?",
                                  r->uri);
                    access_status = HTTP_INTERNAL_SERVER_ERROR;
                    return decl_die(access_status, "check user", r);
                }
                if ((access_status = ap_run_auth_checker(r)) != OK) {
                     return decl_die(access_status, "check authorization", r);
                 }
             }
            else if (access_status == OK) {
                ap_log_rerror(APLOG_MARK, APLOG_TRACE3, 0, r,
                              "request authorized without authentication by "
                              "access_checker_ex hook: %s", r->uri);
            }
            else {
                return decl_die(access_status, "check access", r);
            }
             break;
         case SATISFY_ANY:
             if ((access_status = ap_run_access_checker(r)) == OK) {
                ap_log_rerror(APLOG_MARK, APLOG_TRACE3, 0, r,
                              "request authorized without authentication by "
                              "access_checker hook and 'Satisfy any': %s",
                              r->uri);
                break;
             }
 
             access_status = ap_run_access_checker_ex(r);
            if (access_status == DECLINED
                || (access_status == OK && ap_run_force_authn(r) == OK)) {
                 if ((access_status = ap_run_check_user_id(r)) != OK) {
                     return decl_die(access_status, "check user", r);
                 }
                if (r->user == NULL) {
                    /* don't let buggy authn module crash us in authz */
                    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00028)
                                  "No authentication done but request not "
                                  "allowed without authentication for %s. "
                                  "Authentication not configured?",
                                  r->uri);
                    access_status = HTTP_INTERNAL_SERVER_ERROR;
                    return decl_die(access_status, "check user", r);
                }
                if ((access_status = ap_run_auth_checker(r)) != OK) {
                     return decl_die(access_status, "check authorization", r);
                 }
             }
            else if (access_status == OK) {
                ap_log_rerror(APLOG_MARK, APLOG_TRACE3, 0, r,
                              "request authorized without authentication by "
                              "access_checker_ex hook: %s", r->uri);
            }
            else {
                return decl_die(access_status, "check access", r);
            }
             break;
         }
     }
    /* XXX Must make certain the ap_run_type_checker short circuits mime
     * in mod-proxy for r->proxyreq && r->parsed_uri.scheme
     *                              && !strcmp(r->parsed_uri.scheme, "http")
     */
    if ((access_status = ap_run_type_checker(r)) != OK) {
        return decl_die(access_status, "find types", r);
    }

    if ((access_status = ap_run_fixups(r)) != OK) {
        ap_log_rerror(APLOG_MARK, APLOG_TRACE3, 0, r, "fixups hook gave %d: %s",
                      access_status, r->uri);
        return access_status;
    }

    return OK;
}
