static int x509_crt_verify_child(
                mbedtls_x509_crt *child, mbedtls_x509_crt *parent,
                mbedtls_x509_crt *trust_ca, mbedtls_x509_crl *ca_crl,
                const mbedtls_x509_crt_profile *profile,
                int path_cnt, int self_cnt, uint32_t *flags,
                int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                void *p_vrfy )
{
    int ret;
    uint32_t parent_flags = 0;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    mbedtls_x509_crt *grandparent;
    const mbedtls_md_info_t *md_info;

    /* Counting intermediate self signed certificates */
    if( ( path_cnt != 0 ) && x509_name_cmp( &child->issuer, &child->subject ) == 0 )
        self_cnt++;

     /* path_cnt is 0 for the first intermediate CA */
     if( 1 + path_cnt > MBEDTLS_X509_MAX_INTERMEDIATE_CA )
     {
        /* return immediately as the goal is to avoid unbounded recursion */
        return( MBEDTLS_ERR_X509_FATAL_ERROR );
     }
 
     if( mbedtls_x509_time_is_past( &child->valid_to ) )
        *flags |= MBEDTLS_X509_BADCERT_EXPIRED;

    if( mbedtls_x509_time_is_future( &child->valid_from ) )
        *flags |= MBEDTLS_X509_BADCERT_FUTURE;

    if( x509_profile_check_md_alg( profile, child->sig_md ) != 0 )
        *flags |= MBEDTLS_X509_BADCERT_BAD_MD;

    if( x509_profile_check_pk_alg( profile, child->sig_pk ) != 0 )
        *flags |= MBEDTLS_X509_BADCERT_BAD_PK;

    md_info = mbedtls_md_info_from_type( child->sig_md );
    if( md_info == NULL )
    {
        /*
         * Cannot check 'unknown' hash
         */
        *flags |= MBEDTLS_X509_BADCERT_NOT_TRUSTED;
    }
    else
    {
        mbedtls_md( md_info, child->tbs.p, child->tbs.len, hash );

        if( x509_profile_check_key( profile, child->sig_pk, &parent->pk ) != 0 )
            *flags |= MBEDTLS_X509_BADCERT_BAD_KEY;

        if( mbedtls_pk_verify_ext( child->sig_pk, child->sig_opts, &parent->pk,
                           child->sig_md, hash, mbedtls_md_get_size( md_info ),
                           child->sig.p, child->sig.len ) != 0 )
        {
            *flags |= MBEDTLS_X509_BADCERT_NOT_TRUSTED;
        }
    }

#if defined(MBEDTLS_X509_CRL_PARSE_C)
    /* Check trusted CA's CRL for the given crt */
    *flags |= x509_crt_verifycrl(child, parent, ca_crl, profile );
#endif

    /* Look for a grandparent in trusted CAs */
    for( grandparent = trust_ca;
         grandparent != NULL;
         grandparent = grandparent->next )
    {
        if( x509_crt_check_parent( parent, grandparent,
                                   0, path_cnt == 0 ) == 0 )
            break;
    }

    if( grandparent != NULL )
    {
        ret = x509_crt_verify_top( parent, grandparent, ca_crl, profile,
                                path_cnt + 1, self_cnt, &parent_flags, f_vrfy, p_vrfy );
        if( ret != 0 )
            return( ret );
    }
    else
    {
        /* Look for a grandparent upwards the chain */
        for( grandparent = parent->next;
             grandparent != NULL;
             grandparent = grandparent->next )
        {
            /* +2 because the current step is not yet accounted for
             * and because max_pathlen is one higher than it should be.
             * Also self signed certificates do not count to the limit. */
            if( grandparent->max_pathlen > 0 &&
                grandparent->max_pathlen < 2 + path_cnt - self_cnt )
            {
                continue;
            }

            if( x509_crt_check_parent( parent, grandparent,
                                       0, path_cnt == 0 ) == 0 )
                break;
        }

        /* Is our parent part of the chain or at the top? */
        if( grandparent != NULL )
        {
            ret = x509_crt_verify_child( parent, grandparent, trust_ca, ca_crl,
                                         profile, path_cnt + 1, self_cnt, &parent_flags,
                                         f_vrfy, p_vrfy );
            if( ret != 0 )
                return( ret );
        }
        else
        {
            ret = x509_crt_verify_top( parent, trust_ca, ca_crl, profile,
                                       path_cnt + 1, self_cnt, &parent_flags,
                                       f_vrfy, p_vrfy );
            if( ret != 0 )
                return( ret );
        }
    }

    /* child is verified to be a child of the parent, call verify callback */
    if( NULL != f_vrfy )
        if( ( ret = f_vrfy( p_vrfy, child, path_cnt, flags ) ) != 0 )
            return( ret );

    *flags |= parent_flags;

    return( 0 );
}
