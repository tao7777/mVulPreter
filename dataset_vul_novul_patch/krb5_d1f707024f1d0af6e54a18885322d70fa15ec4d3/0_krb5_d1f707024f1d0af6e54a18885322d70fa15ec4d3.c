krb5_ldap_get_password_policy_from_dn(krb5_context context, char *pol_name,
                                      char *pol_dn, osa_policy_ent_t *policy)
{
    krb5_error_code             st=0, tempst=0;
    LDAP                        *ld=NULL;
    LDAPMessage                 *result=NULL,*ent=NULL;
    kdb5_dal_handle             *dal_handle=NULL;
    krb5_ldap_context           *ldap_context=NULL;
    krb5_ldap_server_handle     *ldap_server_handle=NULL;

    /* Clear the global error string */
    krb5_clear_error_message(context);

    /* validate the input parameters */
    if (pol_dn == NULL)
        return EINVAL;

    *policy = NULL;
    SETUP_CONTEXT();
    GET_HANDLE();

    *(policy) = (osa_policy_ent_t) malloc(sizeof(osa_policy_ent_rec));
    if (*policy == NULL) {
        st = ENOMEM;
        goto cleanup;
    }
    memset(*policy, 0, sizeof(osa_policy_ent_rec));

     LDAP_SEARCH(pol_dn, LDAP_SCOPE_BASE, "(objectclass=krbPwdPolicy)", password_policy_attributes);
 
     ent=ldap_first_entry(ld, result);
    if (ent == NULL) {
        st = KRB5_KDB_NOENTRY;
        goto cleanup;
     }
    st = populate_policy(context, ld, ent, pol_name, *policy);
 
 cleanup:
     ldap_msgfree(result);
    if (st != 0) {
        if (*policy != NULL) {
            krb5_ldap_free_password_policy(context, *policy);
            *policy = NULL;
        }
    }

    krb5_ldap_put_handle_to_pool(ldap_context, ldap_server_handle);
    return st;
}
