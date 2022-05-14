process_db_args(krb5_context context, char **db_args, xargs_t *xargs,
                OPERATION optype)
{
    int                   i=0;
    krb5_error_code       st=0;
    char                  *arg=NULL, *arg_val=NULL;
    char                  **dptr=NULL;
    unsigned int          arg_val_len=0;

     if (db_args) {
         for (i=0; db_args[i]; ++i) {
             arg = strtok_r(db_args[i], "=", &arg_val);
             if (strcmp(arg, TKTPOLICY_ARG) == 0) {
                 dptr = &xargs->tktpolicydn;
             } else {
                if (strcmp(arg, USERDN_ARG) == 0) {
                    if (optype == MODIFY_PRINCIPAL ||
                        xargs->dn != NULL || xargs->containerdn != NULL ||
                        xargs->linkdn != NULL) {
                        st = EINVAL;
                        k5_setmsg(context, st, _("%s option not supported"),
                                  arg);
                        goto cleanup;
                    }
                    dptr = &xargs->dn;
                } else if (strcmp(arg, CONTAINERDN_ARG) == 0) {
                    if (optype == MODIFY_PRINCIPAL ||
                        xargs->dn != NULL || xargs->containerdn != NULL) {
                        st = EINVAL;
                        k5_setmsg(context, st, _("%s option not supported"),
                                  arg);
                        goto cleanup;
                    }
                    dptr = &xargs->containerdn;
                } else if (strcmp(arg, LINKDN_ARG) == 0) {
                    if (xargs->dn != NULL || xargs->linkdn != NULL) {
                        st = EINVAL;
                        k5_setmsg(context, st, _("%s option not supported"),
                                  arg);
                        goto cleanup;
                    }
                    dptr = &xargs->linkdn;
                } else {
                    st = EINVAL;
                    k5_setmsg(context, st, _("unknown option: %s"), arg);
                    goto cleanup;
                }

                xargs->dn_from_kbd = TRUE;
                if (arg_val == NULL || strlen(arg_val) == 0) {
                    st = EINVAL;
                    k5_setmsg(context, st, _("%s option value missing"), arg);
                    goto cleanup;
                }
            }

            if (arg_val == NULL) {
                st = EINVAL;
                k5_setmsg(context, st, _("%s option value missing"), arg);
                goto cleanup;
            }
            arg_val_len = strlen(arg_val) + 1;

            if (strcmp(arg, TKTPOLICY_ARG) == 0) {
                if ((st = krb5_ldap_name_to_policydn (context,
                                                      arg_val,
                                                      dptr)) != 0)
                    goto cleanup;
            } else {
                *dptr = k5memdup(arg_val, arg_val_len, &st);
                if (*dptr == NULL)
                    goto cleanup;
            }
        }
    }

cleanup:
    return st;
}
