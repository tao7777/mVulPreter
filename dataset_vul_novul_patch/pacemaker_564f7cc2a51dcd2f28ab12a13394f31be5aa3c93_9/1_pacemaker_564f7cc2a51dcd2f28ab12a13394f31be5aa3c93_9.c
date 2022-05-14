cib_tls_close(cib_t * cib)
 {
     cib_remote_opaque_t *private = cib->variant_opaque;
 
    shutdown(private->command.socket, SHUT_RDWR);       /* no more receptions */
    shutdown(private->callback.socket, SHUT_RDWR);      /* no more receptions */
    close(private->command.socket);
    close(private->callback.socket);
 #ifdef HAVE_GNUTLS_GNUTLS_H
     if (private->command.encrypted) {
        gnutls_bye(*(private->command.session), GNUTLS_SHUT_RDWR);
        gnutls_deinit(*(private->command.session));
        gnutls_free(private->command.session);
        gnutls_bye(*(private->callback.session), GNUTLS_SHUT_RDWR);
        gnutls_deinit(*(private->callback.session));
        gnutls_free(private->callback.session);
 
        gnutls_anon_free_client_credentials(anon_cred_c);
        gnutls_global_deinit();
     }
 #endif
     return 0;
 }
