untrusted_launcher_response_callback (GtkDialog                 *dialog,
                                      int                        response_id,
                                      ActivateParametersDesktop *parameters)
{
    GdkScreen *screen;
    char *uri;
    GFile *file;
 
     switch (response_id)
     {
        case GTK_RESPONSE_OK:
         {
            file = nautilus_file_get_location (parameters->file);

            /* We need to do this in order to prevent malicious desktop files
             * with the executable bit already set.
             * See https://bugzilla.gnome.org/show_bug.cgi?id=777991
             */
            nautilus_file_set_metadata (parameters->file, NAUTILUS_METADATA_KEY_DESKTOP_FILE_TRUSTED,
                                        NULL,
                                        "yes");

            nautilus_file_mark_desktop_file_executable (file,
                                                        parameters->parent_window,
                                                        TRUE,
                                                        NULL, NULL);

            /* Need to force a reload of the attributes so is_trusted is marked
             * correctly. Not sure why the general monitor doesn't fire in this
             * case when setting the metadata
             */
            nautilus_file_invalidate_all_attributes (parameters->file);

             screen = gtk_widget_get_screen (GTK_WIDGET (parameters->parent_window));
             uri = nautilus_file_get_uri (parameters->file);
             DEBUG ("Launching untrusted launcher %s", uri);
             nautilus_launch_desktop_file (screen, uri, NULL,
                                           parameters->parent_window);
             g_free (uri);
             g_object_unref (file);
         }
         break;

        default:
        {
            /* Just destroy dialog */
        }
        break;
    }

    gtk_widget_destroy (GTK_WIDGET (dialog));
    activate_parameters_desktop_free (parameters);
}
