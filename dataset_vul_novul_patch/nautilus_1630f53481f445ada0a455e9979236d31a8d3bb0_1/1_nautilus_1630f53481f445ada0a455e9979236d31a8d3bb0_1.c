mark_desktop_file_trusted (CommonJob    *common,
                           GCancellable *cancellable,
                           GFile        *file,
                           gboolean      interactive)
 {
    char *contents, *new_contents;
    gsize length, new_length;
     GError *error;
     guint32 current_perms, new_perms;
     int response;
     GFileInfo *info;
 
 retry:
    error = NULL;
    if (!g_file_load_contents (file,
                               cancellable,
                               &contents, &length,
                               NULL, &error))
    {
        if (interactive)
        {
            response = run_error (common,
                                  g_strdup (_("Unable to mark launcher trusted (executable)")),
                                  error->message,
                                  NULL,
                                  FALSE,
                                  CANCEL, RETRY,
                                  NULL);
        }
        else
        {
            response = 0;
        }
        if (response == 0 || response == GTK_RESPONSE_DELETE_EVENT)
        {
            abort_job (common);
        }
        else if (response == 1)
        {
            goto retry;
        }
        else
        {
            g_assert_not_reached ();
        }
        goto out;
    }
    if (!g_str_has_prefix (contents, "#!"))
    {
        new_length = length + strlen (TRUSTED_SHEBANG);
        new_contents = g_malloc (new_length);
        strcpy (new_contents, TRUSTED_SHEBANG);
        memcpy (new_contents + strlen (TRUSTED_SHEBANG),
                contents, length);
        if (!g_file_replace_contents (file,
                                      new_contents,
                                      new_length,
                                      NULL,
                                      FALSE, 0,
                                      NULL, cancellable, &error))
        {
            g_free (contents);
            g_free (new_contents);
            if (interactive)
            {
                response = run_error (common,
                                      g_strdup (_("Unable to mark launcher trusted (executable)")),
                                      error->message,
                                      NULL,
                                      FALSE,
                                      CANCEL, RETRY,
                                      NULL);
            }
            else
            {
                response = 0;
            }
            if (response == 0 || response == GTK_RESPONSE_DELETE_EVENT)
            {
                abort_job (common);
            }
            else if (response == 1)
            {
                goto retry;
            }
            else
            {
                g_assert_not_reached ();
            }
            goto out;
        }
        g_free (new_contents);
    }
    g_free (contents);
 
     info = g_file_query_info (file,
                               G_FILE_ATTRIBUTE_STANDARD_TYPE ","
                               G_FILE_ATTRIBUTE_UNIX_MODE,
                              G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                              common->cancellable,
                              &error);

    if (info == NULL)
    {
        if (interactive)
        {
            response = run_error (common,
                                  g_strdup (_("Unable to mark launcher trusted (executable)")),
                                  error->message,
                                  NULL,
                                  FALSE,
                                  CANCEL, RETRY,
                                  NULL);
        }
        else
        {
            response = 0;
        }

        if (response == 0 || response == GTK_RESPONSE_DELETE_EVENT)
        {
            abort_job (common);
        }
        else if (response == 1)
        {
            goto retry;
        }
        else
        {
            g_assert_not_reached ();
        }

        goto out;
    }


    if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_UNIX_MODE))
    {
        current_perms = g_file_info_get_attribute_uint32 (info, G_FILE_ATTRIBUTE_UNIX_MODE);
        new_perms = current_perms | S_IXGRP | S_IXUSR | S_IXOTH;

        if ((current_perms != new_perms) &&
            !g_file_set_attribute_uint32 (file, G_FILE_ATTRIBUTE_UNIX_MODE,
                                          new_perms, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                          common->cancellable, &error))
        {
            g_object_unref (info);

            if (interactive)
            {
                response = run_error (common,
                                      g_strdup (_("Unable to mark launcher trusted (executable)")),
                                      error->message,
                                      NULL,
                                      FALSE,
                                      CANCEL, RETRY,
                                      NULL);
            }
            else
            {
                response = 0;
            }

            if (response == 0 || response == GTK_RESPONSE_DELETE_EVENT)
            {
                abort_job (common);
            }
            else if (response == 1)
            {
                goto retry;
            }
            else
            {
                g_assert_not_reached ();
            }

            goto out;
        }
    }
    g_object_unref (info);
out:
    ;
 }
