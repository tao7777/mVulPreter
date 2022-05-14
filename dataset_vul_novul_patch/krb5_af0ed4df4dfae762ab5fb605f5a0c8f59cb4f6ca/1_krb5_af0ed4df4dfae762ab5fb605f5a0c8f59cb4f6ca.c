kadm5_randkey_principal_3(void *server_handle,
                          krb5_principal principal,
                          krb5_boolean keepold,
                          int n_ks_tuple, krb5_key_salt_tuple *ks_tuple,
                          krb5_keyblock **keyblocks,
                          int *n_keys)
{
    krb5_db_entry               *kdb;
     osa_princ_ent_rec           adb;
     krb5_int32                  now;
     kadm5_policy_ent_rec        pol;
    int                         ret, last_pwd;
     krb5_boolean                have_pol = FALSE;
     kadm5_server_handle_t       handle = server_handle;
     krb5_keyblock               *act_mkey;
    krb5_kvno                   act_kvno;
    int                         new_n_ks_tuple = 0;
    krb5_key_salt_tuple         *new_ks_tuple = NULL;

    if (keyblocks)
        *keyblocks = NULL;

    CHECK_HANDLE(server_handle);

    krb5_clear_error_message(handle->context);

    if (principal == NULL)
        return EINVAL;

    if ((ret = kdb_get_entry(handle, principal, &kdb, &adb)))
        return(ret);

    ret = apply_keysalt_policy(handle, adb.policy, n_ks_tuple, ks_tuple,
                               &new_n_ks_tuple, &new_ks_tuple);
    if (ret)
        goto done;

    if (krb5_principal_compare(handle->context, principal, hist_princ)) {
        /* If changing the history entry, the new entry must have exactly one
         * key. */
        if (keepold)
            return KADM5_PROTECT_PRINCIPAL;
        new_n_ks_tuple = 1;
    }

    ret = kdb_get_active_mkey(handle, &act_kvno, &act_mkey);
    if (ret)
        goto done;

    ret = krb5_dbe_crk(handle->context, act_mkey, new_ks_tuple, new_n_ks_tuple,
                       keepold, kdb);
    if (ret)
        goto done;

    ret = krb5_dbe_update_mkvno(handle->context, kdb, act_kvno);
    if (ret)
        goto done;

    kdb->attributes &= ~KRB5_KDB_REQUIRES_PWCHANGE;

    ret = krb5_timeofday(handle->context, &now);
    if (ret)
        goto done;

    if ((adb.aux_attributes & KADM5_POLICY)) {
        ret = get_policy(handle, adb.policy, &pol, &have_pol);
        if (ret)
            goto done;
    }
    if (have_pol) {
        ret = krb5_dbe_lookup_last_pwd_change(handle->context, kdb, &last_pwd);
        if (ret)
            goto done;

#if 0
        /*
         * The spec says this check is overridden if the caller has
         * modify privilege.  The admin server therefore makes this
         * check itself (in chpass_principal_wrapper, misc.c).  A
         * local caller implicitly has all authorization bits.
         */
        if((now - last_pwd) < pol.pw_min_life &&
           !(kdb->attributes & KRB5_KDB_REQUIRES_PWCHANGE)) {
            ret = KADM5_PASS_TOOSOON;
            goto done;
        }
#endif

        if (pol.pw_max_life)
            kdb->pw_expiration = now + pol.pw_max_life;
        else
            kdb->pw_expiration = 0;
    } else {
        kdb->pw_expiration = 0;
    }

    ret = krb5_dbe_update_last_pwd_change(handle->context, kdb, now);
    if (ret)
        goto done;

    /* unlock principal on this KDC */
     kdb->fail_auth_count = 0;
 
     if (keyblocks) {
        ret = decrypt_key_data(handle->context,
                               kdb->n_key_data, kdb->key_data,
                                keyblocks, n_keys);
         if (ret)
             goto done;
    }

    /* key data changed, let the database provider know */
    kdb->mask = KADM5_KEY_DATA | KADM5_FAIL_AUTH_COUNT;
    /* | KADM5_RANDKEY_USED */;

    ret = k5_kadm5_hook_chpass(handle->context, handle->hook_handles,
                               KADM5_HOOK_STAGE_PRECOMMIT, principal, keepold,
                               new_n_ks_tuple, new_ks_tuple, NULL);
    if (ret)
        goto done;
    if ((ret = kdb_put_entry(handle, kdb, &adb)))
        goto done;

    (void) k5_kadm5_hook_chpass(handle->context, handle->hook_handles,
                                KADM5_HOOK_STAGE_POSTCOMMIT, principal,
                                keepold, new_n_ks_tuple, new_ks_tuple, NULL);
    ret = KADM5_OK;
done:
    free(new_ks_tuple);
    kdb_free_entry(handle, kdb, &adb);
    if (have_pol)
        kadm5_free_policy_ent(handle->lhandle, &pol);

    return ret;
}
