mark_desktop_file_trusted (CommonJob    *common,
mark_desktop_file_executable (CommonJob    *common,
                              GCancellable *cancellable,
                              GFile        *file,
                              gboolean      interactive)
 {
     GError *error;
     guint32 current_perms, new_perms;
     int response;
     GFileInfo *info;
 
 retry:
 
    error = NULL;
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
