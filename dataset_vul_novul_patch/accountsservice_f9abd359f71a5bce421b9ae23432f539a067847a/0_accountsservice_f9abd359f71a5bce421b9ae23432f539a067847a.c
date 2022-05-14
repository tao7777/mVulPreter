user_change_icon_file_authorized_cb (Daemon                *daemon,
                                     User                  *user,
                                     GDBusMethodInvocation *context,
                                     gpointer               data)

{
        g_autofree gchar *filename = NULL;
        g_autoptr(GFile) file = NULL;
        g_autoptr(GFileInfo) info = NULL;
        guint32 mode;
        GFileType type;
        guint64 size;

        filename = g_strdup (data);

        if (filename == NULL ||
            *filename == '\0') {
                g_autofree gchar *dest_path = NULL;
                g_autoptr(GFile) dest = NULL;
                g_autoptr(GError) error = NULL;

                g_clear_pointer (&filename, g_free);

                dest_path = g_build_filename (ICONDIR, accounts_user_get_user_name (ACCOUNTS_USER (user)), NULL);
                dest = g_file_new_for_path (dest_path);

                if (!g_file_delete (dest, NULL, &error) &&
                    !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND)) {
                        throw_error (context, ERROR_FAILED, "failed to remove user icon, %s", error->message);
                        return;
                }
                goto icon_saved;
         }
 
         file = g_file_new_for_path (filename);
        g_clear_pointer (&filename, g_free);

        /* Canonicalize path so we can call g_str_has_prefix on it
         * below without concern for ../ path components moving outside
         * the prefix
         */
        filename = g_file_get_path (file);

         info = g_file_query_info (file, G_FILE_ATTRIBUTE_UNIX_MODE ","
                                         G_FILE_ATTRIBUTE_STANDARD_TYPE ","
                                         G_FILE_ATTRIBUTE_STANDARD_SIZE,
                return;
        }
