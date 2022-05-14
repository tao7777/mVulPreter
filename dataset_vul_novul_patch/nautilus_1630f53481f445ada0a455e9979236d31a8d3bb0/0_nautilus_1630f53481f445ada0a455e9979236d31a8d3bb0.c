is_link_trusted (NautilusFile *file,
                 gboolean      is_launcher)
 {
     GFile *location;
     gboolean res;
    g_autofree gchar* trusted = NULL;
 
     if (!is_launcher)
     {
         return TRUE;
     }
 
    trusted = nautilus_file_get_metadata (file,
                                          NAUTILUS_METADATA_KEY_DESKTOP_FILE_TRUSTED,
                                          NULL);
    if (nautilus_file_can_execute (file) && trusted != NULL)
     {
         return TRUE;
     }

    res = FALSE;

    if (nautilus_file_is_local (file))
    {
        location = nautilus_file_get_location (file);
        res = nautilus_is_in_system_dir (location);
        g_object_unref (location);
    }

    return res;
}
