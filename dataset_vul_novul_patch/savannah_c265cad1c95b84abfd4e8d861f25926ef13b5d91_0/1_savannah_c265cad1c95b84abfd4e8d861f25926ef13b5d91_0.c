network_init ()
{
#ifdef HAVE_GNUTLS
    char *ca_path, *ca_path2;

    gnutls_global_init ();
    gnutls_certificate_allocate_credentials (&gnutls_xcred);
    
    ca_path = string_expand_home (CONFIG_STRING(config_network_gnutls_ca_file));
    if (ca_path)
    {
        ca_path2 = string_replace (ca_path, "%h", weechat_home);
        if (ca_path2)
        {
            gnutls_certificate_set_x509_trust_file (gnutls_xcred, ca_path2,
                                                    GNUTLS_X509_FMT_PEM);
            free (ca_path2);
         }
         free (ca_path);
     }
     gnutls_certificate_client_set_retrieve_function (gnutls_xcred,
                                                      &hook_connect_gnutls_set_certificates);
     network_init_ok = 1;
    gcry_check_version (GCRYPT_VERSION);
    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
#endif
}
