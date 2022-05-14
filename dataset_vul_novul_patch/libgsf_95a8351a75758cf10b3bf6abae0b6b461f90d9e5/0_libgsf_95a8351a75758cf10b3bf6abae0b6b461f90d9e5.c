tar_directory_for_file (GsfInfileTar *dir, const char *name, gboolean last)
{
	const char *s = name;

	while (1) {
		const char *s0 = s;
		char *dirname;

		/* Find a directory component, if any.  */
		while (1) {
			if (*s == 0) {
				if (last && s != s0)
					break;
				else
					return dir;
			}
			/* This is deliberately slash-only.  */
			if (*s == '/')
				break;
			s++;
		}

		dirname = g_strndup (s0, s - s0);
		while (*s == '/')
			s++;

		if (strcmp (dirname, ".") != 0) {
			GsfInput *subdir =
 				gsf_infile_child_by_name (GSF_INFILE (dir),
 							  dirname);
 			if (subdir) {
				dir = GSF_IS_INFILE_TAR (subdir)
					? GSF_INFILE_TAR (subdir)
					: dir;
 				/* Undo the ref. */
 				g_object_unref (subdir);
 			} else
 				dir = tar_create_dir (dir, dirname);
 		}

		g_free (dirname);
	}
}
