comics_check_decompress_command	(gchar          *mime_type, 
				 ComicsDocument *comics_document,
				 GError         **error)
{
	gboolean success;
	gchar *std_out, *std_err;
	gint retval;
	GError *err = NULL;
	
	/* FIXME, use proper cbr/cbz mime types once they're
	 * included in shared-mime-info */
	
	if (g_content_type_is_a (mime_type, "application/x-cbr") ||
	    g_content_type_is_a (mime_type, "application/x-rar")) {
	        /* The RARLAB provides a no-charge proprietary (freeware) 
	        * decompress-only client for Linux called unrar. Another 
		* option is a GPLv2-licensed command-line tool developed by 
		* the Gna! project. Confusingly enough, the free software RAR 
		* decoder is also named unrar. For this reason we need to add 
		* some lines for disambiguation. Sorry for the added the 
		* complexity but it's life :)
		* Finally, some distributions, like Debian, rename this free 
		* option as unrar-free. 
		* */
		comics_document->selected_command = 
					g_find_program_in_path ("unrar");
		if (comics_document->selected_command) {
			/* We only use std_err to avoid printing useless error 
			 * messages on the terminal */
			success = 
				g_spawn_command_line_sync (
				              comics_document->selected_command, 
							   &std_out, &std_err,
							   &retval, &err);
			if (!success) {
				g_propagate_error (error, err);
				g_error_free (err);
				return FALSE;
			/* I don't check retval status because RARLAB unrar 
			 * doesn't have a way to return 0 without involving an 
			 * operation with a file*/
			} else if (WIFEXITED (retval)) {
				if (g_strrstr (std_out,"freeware") != NULL)
					/* The RARLAB freeware client */
					comics_document->command_usage = RARLABS;
				else
					/* The Gna! free software client */
					comics_document->command_usage = GNAUNRAR;

				g_free (std_out);
				g_free (std_err);
				return TRUE;
			}
		}
		/* The Gna! free software client with Debian naming convention */
		comics_document->selected_command = 
				g_find_program_in_path ("unrar-free");
		if (comics_document->selected_command) {
 			comics_document->command_usage = GNAUNRAR;
 			return TRUE;
 		}
 	} else if (g_content_type_is_a (mime_type, "application/x-cbz") ||
 		   g_content_type_is_a (mime_type, "application/zip")) {
 		/* InfoZIP's unzip program */
		comics_document->selected_command = 
				g_find_program_in_path ("unzip");
		comics_document->alternative_command =
				g_find_program_in_path ("zipnote");
		if (comics_document->selected_command &&
		    comics_document->alternative_command) {
			comics_document->command_usage = UNZIP;
			return TRUE;
		}
		/* fallback mode using 7za and 7z from p7zip project  */
		comics_document->selected_command =
				g_find_program_in_path ("7za");
		if (comics_document->selected_command) {
			comics_document->command_usage = P7ZIP;
			return TRUE;
		}
		comics_document->selected_command =
				g_find_program_in_path ("7z");
		if (comics_document->selected_command) {
 			comics_document->command_usage = P7ZIP;
 			return TRUE;
 		}
 
 	} else if (g_content_type_is_a (mime_type, "application/x-cb7") ||
 		   g_content_type_is_a (mime_type, "application/x-7z-compressed")) {
		/* 7zr, 7za and 7z are the commands from the p7zip project able 
		 * to decompress .7z files */ 
		comics_document->selected_command =
				g_find_program_in_path ("7zr");
		if (comics_document->selected_command) {
			comics_document->command_usage = P7ZIP;
			return TRUE;
		}
		comics_document->selected_command =
				g_find_program_in_path ("7za");
		if (comics_document->selected_command) {
			comics_document->command_usage = P7ZIP;
			return TRUE;
		}
		comics_document->selected_command =
				g_find_program_in_path ("7z");
		if (comics_document->selected_command) {
 			comics_document->command_usage = P7ZIP;
 			return TRUE;
 		}
 	} else {
 		g_set_error (error,
 			     EV_DOCUMENT_ERROR,
			     EV_DOCUMENT_ERROR_INVALID,
			     _("Not a comic book MIME type: %s"),
			     mime_type);
			     return FALSE;
	}
	g_set_error_literal (error,
			     EV_DOCUMENT_ERROR,
			     EV_DOCUMENT_ERROR_INVALID,
			     _("Can’t find an appropriate command to "
			     "decompress this type of comic book"));
	return FALSE;
}
