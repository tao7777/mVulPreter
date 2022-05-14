create_tls_session(int csock, int type /* GNUTLS_SERVER, GNUTLS_CLIENT */ )
void *
crm_create_anon_tls_session(int csock, int type /* GNUTLS_SERVER, GNUTLS_CLIENT */, void *credentials)
 {
     gnutls_session *session = gnutls_malloc(sizeof(gnutls_session));
 
     gnutls_init(session, type);
#  ifdef HAVE_GNUTLS_PRIORITY_SET_DIRECT
/*      http://www.manpagez.com/info/gnutls/gnutls-2.10.4/gnutls_81.php#Echo-Server-with-anonymous-authentication */
    gnutls_priority_set_direct(*session, "NORMAL:+ANON-DH", NULL);
 /*	gnutls_priority_set_direct (*session, "NONE:+VERS-TLS-ALL:+CIPHER-ALL:+MAC-ALL:+SIGN-ALL:+COMP-ALL:+ANON-DH", NULL); */
 #  else
     gnutls_set_default_priority(*session);
    gnutls_kx_set_priority(*session, anon_tls_kx_order);
 #  endif
     gnutls_transport_set_ptr(*session, (gnutls_transport_ptr) GINT_TO_POINTER(csock));
     switch (type) {
    case GNUTLS_SERVER:
        gnutls_credentials_set(*session, GNUTLS_CRD_ANON, (gnutls_anon_server_credentials_t) credentials);
        break;
    case GNUTLS_CLIENT:
        gnutls_credentials_set(*session, GNUTLS_CRD_ANON, (gnutls_anon_client_credentials_t) credentials);
        break;
     }
 
     return session;
 }
