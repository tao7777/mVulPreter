inf_gtk_certificate_manager_certificate_func(InfXmppConnection* connection,
                                             gnutls_session_t session,
                                             InfCertificateChain* chain,
                                             gpointer user_data)
{
  InfGtkCertificateManager* manager;
  InfGtkCertificateManagerPrivate* priv;

  InfGtkCertificateDialogFlags flags;
  gnutls_x509_crt_t presented_cert;
  gnutls_x509_crt_t known_cert;
  gchar* hostname;

  gboolean match_hostname;
  gboolean issuer_known;
  gnutls_x509_crt_t root_cert;

  int ret;
  unsigned int verify;
  GHashTable* table;
  gboolean cert_equal;
  time_t expiration_time;

  InfGtkCertificateManagerQuery* query;
  gchar* text;
  GtkWidget* vbox;
  GtkWidget* label;

  GError* error;

  manager = INF_GTK_CERTIFICATE_MANAGER(user_data);
  priv = INF_GTK_CERTIFICATE_MANAGER_PRIVATE(manager);

  g_object_get(G_OBJECT(connection), "remote-hostname", &hostname, NULL);
  presented_cert = inf_certificate_chain_get_own_certificate(chain);

  match_hostname = gnutls_x509_crt_check_hostname(presented_cert, hostname);

  /* First, validate the certificate */
  ret = gnutls_certificate_verify_peers2(session, &verify);
  error = NULL;

  if(ret != GNUTLS_E_SUCCESS)
    inf_gnutls_set_error(&error, ret);

  /* Remove the GNUTLS_CERT_ISSUER_NOT_KNOWN flag from the verification
   * result, and if the certificate is still invalid, then set an error. */
  if(error == NULL)
  {
    issuer_known = TRUE;
    if(verify & GNUTLS_CERT_SIGNER_NOT_FOUND)
    {
      issuer_known = FALSE;

      /* Re-validate the certificate for other failure reasons --
       * unfortunately the gnutls_certificate_verify_peers2() call
       * does not tell us whether the certificate is otherwise invalid
       * if a signer is not found already. */
      /* TODO: Here it would be good to use the verify flags from the
       * certificate credentials, but GnuTLS does not have API to
       * retrieve them. */
      root_cert = inf_certificate_chain_get_root_certificate(chain);

      ret = gnutls_x509_crt_list_verify(
        inf_certificate_chain_get_raw(chain),
        inf_certificate_chain_get_n_certificates(chain),
        &root_cert,
        1,
        NULL,
        0,
        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT,
        &verify
      );
 
       if(ret != GNUTLS_E_SUCCESS)
         inf_gnutls_set_error(&error, ret);
     }

    if(error == NULL)
      if(verify & GNUTLS_CERT_INVALID)
        inf_gnutls_certificate_verification_set_error(&error, verify);
   }
 
   /* Look up the host in our database of pinned certificates if we could not
   * fully verify the certificate, i.e. if either the issuer is not known or
   * the hostname of the connection does not match the certificate. */
  table = NULL;
  if(error == NULL)
  {
    known_cert = NULL;
    if(!match_hostname || !issuer_known)
    {
      /* If we cannot load the known host file, then cancel the connection.
       * Otherwise it might happen that someone shows us a certificate that we
       * tell the user we don't know, if though actually for that host we expect
       * a different certificate. */
      table = inf_gtk_certificate_manager_ref_known_hosts(manager, &error);
      if(table != NULL)
        known_cert = g_hash_table_lookup(table, hostname);
    }
  }

  /* Next, configure the flags for the dialog to be shown based on the
   * verification result, and on whether the pinned certificate matches
   * the one presented by the host or not. */
  flags = 0;
  if(error == NULL)
  {
    if(known_cert != NULL)
    {
      cert_equal = inf_gtk_certificate_manager_compare_fingerprint(
        known_cert,
        presented_cert,
        &error
      );

      if(error == NULL && cert_equal == FALSE)
      {
        if(!match_hostname)
          flags |= INF_GTK_CERTIFICATE_DIALOG_CERT_HOSTNAME_MISMATCH;
        if(!issuer_known)
          flags |= INF_GTK_CERTIFICATE_DIALOG_CERT_ISSUER_NOT_KNOWN;

        flags |= INF_GTK_CERTIFICATE_DIALOG_CERT_UNEXPECTED;
        expiration_time = gnutls_x509_crt_get_expiration_time(known_cert);
        if(expiration_time != (time_t)(-1))
        {
          expiration_time -= INF_GTK_CERTIFICATE_MANAGER_EXPIRATION_TOLERANCE;
          if(time(NULL) > expiration_time)
          {
            flags |= INF_GTK_CERTIFICATE_DIALOG_CERT_OLD_EXPIRED;
          }
        }
      }
    }
    else
    {
      if(!match_hostname)
        flags |= INF_GTK_CERTIFICATE_DIALOG_CERT_HOSTNAME_MISMATCH;
      if(!issuer_known)
        flags |= INF_GTK_CERTIFICATE_DIALOG_CERT_ISSUER_NOT_KNOWN;
    }
  }

  /* Now proceed either by accepting the connection, rejecting it, or
   * bothering the user with an annoying dialog. */
  if(error == NULL)
  {
    if(flags == 0)
    {
      if(match_hostname && issuer_known)
      {
        /* Remove the pinned entry if we now have a valid certificate for
         * this host. */
        if(table != NULL && g_hash_table_remove(table, hostname) == TRUE)
        {
          inf_gtk_certificate_manager_write_known_hosts_with_warning(
            manager,
            table
          );
        }
      }

      inf_xmpp_connection_certificate_verify_continue(connection);
    }
    else
    {
      query = g_slice_new(InfGtkCertificateManagerQuery);
      query->manager = manager;
      query->known_hosts = table;
      query->connection = connection;
      query->dialog = inf_gtk_certificate_dialog_new(
        priv->parent_window,
        0,
        flags,
        hostname,
        chain
      );
      query->certificate_chain = chain;

      table = NULL;

      g_object_ref(query->connection);
      inf_certificate_chain_ref(chain);

      g_signal_connect(
        G_OBJECT(connection),
        "notify::status",
        G_CALLBACK(inf_gtk_certificate_manager_notify_status_cb),
        query
      );

      g_signal_connect(
        G_OBJECT(query->dialog),
        "response",
        G_CALLBACK(inf_gtk_certificate_manager_response_cb),
        query
      );

      gtk_dialog_add_button(
        GTK_DIALOG(query->dialog),
        _("_Cancel connection"),
        GTK_RESPONSE_REJECT
      );

      gtk_dialog_add_button(
        GTK_DIALOG(query->dialog),
        _("C_ontinue connection"),
        GTK_RESPONSE_ACCEPT
      );

      text = g_strdup_printf(
        _("Do you want to continue the connection to host \"%s\"? If you "
          "choose to continue, this certificate will be trusted in the "
          "future when connecting to this host."),
        hostname
      );

      label = gtk_label_new(text);
      gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
      gtk_label_set_line_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD_CHAR);
      gtk_label_set_max_width_chars(GTK_LABEL(label), 60);
      gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
      gtk_widget_show(label);
      g_free(text);

      vbox = gtk_dialog_get_content_area(GTK_DIALOG(query->dialog));
      gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

      priv->queries = g_slist_prepend(priv->queries, query);
      gtk_window_present(GTK_WINDOW(query->dialog));
    }
  }
  else
  {
    inf_xmpp_connection_certificate_verify_cancel(connection, error);
    g_error_free(error);
  }

  if(table != NULL) g_hash_table_unref(table);
  g_free(hostname);
}
