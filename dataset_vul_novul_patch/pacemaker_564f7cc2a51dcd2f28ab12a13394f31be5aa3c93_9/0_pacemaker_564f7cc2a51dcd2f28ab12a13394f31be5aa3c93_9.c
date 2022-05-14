cib_tls_close(cib_t * cib)
 {
     cib_remote_opaque_t *private = cib->variant_opaque;
 
 #ifdef HAVE_GNUTLS_GNUTLS_H
     if (private->command.encrypted) {
        if (private->command.session) {
            gnutls_bye(*(private->command.session), GNUTLS_SHUT_RDWR);
            gnutls_deinit(*(private->command.session));
            gnutls_free(private->command.session);
        }
 
        if (private->callback.session) {
            gnutls_bye(*(private->callback.session), GNUTLS_SHUT_RDWR);
            gnutls_deinit(*(private->callback.session));
            gnutls_free(private->callback.session);
        }
        private->command.session = NULL;
        private->callback.session = NULL;
        if (remote_gnutls_credentials_init) {
            gnutls_anon_free_client_credentials(anon_cred_c);
            gnutls_global_deinit();
            remote_gnutls_credentials_init = FALSE;
        }
     }
 #endif

    if (private->command.socket) {
        shutdown(private->command.socket, SHUT_RDWR);       /* no more receptions */
        close(private->command.socket);
    }
    if (private->callback.socket) {
        shutdown(private->callback.socket, SHUT_RDWR);      /* no more receptions */
        close(private->callback.socket);
    }
    private->command.socket = 0;
    private->callback.socket = 0;

    free(private->command.recv_buf);
    free(private->callback.recv_buf);
    private->command.recv_buf = NULL;
    private->callback.recv_buf = NULL;

     return 0;
 }
