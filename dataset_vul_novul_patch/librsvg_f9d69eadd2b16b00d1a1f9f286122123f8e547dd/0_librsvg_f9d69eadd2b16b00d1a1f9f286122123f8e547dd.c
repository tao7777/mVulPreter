_rsvg_io_get_file_path (const gchar * filename,
                        const gchar * base_uri)
 {
     gchar *absolute_filename;
 
    if (g_path_is_absolute (filename)) {
         absolute_filename = g_strdup (filename);
     } else {
         gchar *tmpcdir;
        gchar *base_filename;

        if (base_uri) {
            base_filename = g_filename_from_uri (base_uri, NULL, NULL);
            if (base_filename != NULL) {
                tmpcdir = g_path_get_dirname (base_filename);
                g_free (base_filename);
            } else 
                return NULL;
        } else
            tmpcdir = g_get_current_dir ();

        absolute_filename = g_build_filename (tmpcdir, filename, NULL);
        g_free (tmpcdir);
    }

    return absolute_filename;
}
