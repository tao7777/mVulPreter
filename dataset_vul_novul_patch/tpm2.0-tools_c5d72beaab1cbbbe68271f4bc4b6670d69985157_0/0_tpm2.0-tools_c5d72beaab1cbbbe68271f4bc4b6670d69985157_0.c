static TPM_RC StartAuthSession(TSS2_SYS_CONTEXT *sapi_context, SESSION *session )
{
    TPM_RC rval;
    TPM2B_ENCRYPTED_SECRET key;
    char label[] = "ATH";
    UINT16 bytes;
    int i;

    key.t.size = 0;

    if( session->nonceOlder.t.size == 0 )
    {
        /* this is an internal routine to TSS and should be removed */
        session->nonceOlder.t.size = GetDigestSize( TPM_ALG_SHA1 );
        for( i = 0; i < session->nonceOlder.t.size; i++ )
            session->nonceOlder.t.buffer[i] = 0;
    }

    session->nonceNewer.t.size = session->nonceOlder.t.size;
    rval = Tss2_Sys_StartAuthSession( sapi_context, session->tpmKey, session->bind, 0,
            &( session->nonceOlder ), &( session->encryptedSalt ), session->sessionType,
            &( session->symmetric ), session->authHash, &( session->sessionHandle ),
            &( session->nonceNewer ), 0 );

    if( rval == TPM_RC_SUCCESS )
    {
        if( session->tpmKey == TPM_RH_NULL )
            session->salt.t.size = 0;
        if( session->bind == TPM_RH_NULL )
            session->authValueBind.t.size = 0;

        if( session->tpmKey == TPM_RH_NULL && session->bind == TPM_RH_NULL )
        {
            session->sessionKey.b.size = 0;
        }
        else
        {
            bool result = string_bytes_concat_buffer( (TPM2B_MAX_BUFFER *)&key, &( session->authValueBind.b ) );
            if (!result)
            {
               return TSS2_SYS_RC_BAD_VALUE;
            }

            result = string_bytes_concat_buffer( (TPM2B_MAX_BUFFER *)&key, &( session->salt.b ) );
            if (!result)
            {
                return TSS2_SYS_RC_BAD_VALUE;
            }

            bytes = GetDigestSize( session->authHash );

            if( key.t.size == 0 )
            {
                session->sessionKey.t.size = 0;
             }
             else
             {
                rval = tpm_kdfa(session->authHash, &(key.b), label, &( session->nonceNewer.b ),
                         &( session->nonceOlder.b ), bytes * 8, (TPM2B_MAX_BUFFER *)&( session->sessionKey ) );
             }
 
            if( rval != TPM_RC_SUCCESS )
            {
                return( TSS2_APP_RC_CREATE_SESSION_KEY_FAILED );
            }
        }

        session->nonceTpmDecrypt.b.size = 0;
        session->nonceTpmEncrypt.b.size = 0;
        session->nvNameChanged = 0;
    }

    return rval;
}
