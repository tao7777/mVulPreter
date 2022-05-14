AP_CORE_DECLARE_NONSTD(const char *) ap_limit_section(cmd_parms *cmd,
                                                      void *dummy,
                                                      const char *arg)
{
    const char *endp = ap_strrchr_c(arg, '>');
    const char *limited_methods;
    void *tog = cmd->cmd->cmd_data;
    apr_int64_t limited = 0;
    apr_int64_t old_limited = cmd->limited;
    const char *errmsg;

    if (endp == NULL) {
        return unclosed_directive(cmd);
    }

    limited_methods = apr_pstrmemdup(cmd->temp_pool, arg, endp - arg);

    if (!limited_methods[0]) {
        return missing_container_arg(cmd);
    }

    while (limited_methods[0]) {
        char *method = ap_getword_conf(cmd->temp_pool, &limited_methods);
        int methnum;

        /* check for builtin or module registered method number */
        methnum = ap_method_number_of(method);

        if (methnum == M_TRACE && !tog) {
            return "TRACE cannot be controlled by <Limit>, see TraceEnable";
        }
        else if (methnum == M_INVALID) {
             /* method has not been registered yet, but resource restriction
              * is always checked before method handling, so register it.
              */
            if (cmd->pool == cmd->temp_pool) {
                /* In .htaccess, we can't globally register new methods. */
                return apr_psprintf(cmd->pool, "Could not register method '%s' "
                                   "for %s from .htaccess configuration",
                                    method, cmd->cmd->name);
            }
             methnum = ap_method_register(cmd->pool,
                                          apr_pstrdup(cmd->pool, method));
         }

        limited |= (AP_METHOD_BIT << methnum);
    }

    /* Killing two features with one function,
     * if (tog == NULL) <Limit>, else <LimitExcept>
     */
    limited = tog ? ~limited : limited;

    if (!(old_limited & limited)) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name,
                           "> directive excludes all methods", NULL);
    }
    else if ((old_limited & limited) == old_limited) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name,
                           "> directive specifies methods already excluded",
                           NULL);
    }

    cmd->limited &= limited;

    errmsg = ap_walk_config(cmd->directive->first_child, cmd, cmd->context);

    cmd->limited = old_limited;

    return errmsg;
}
