email_close(FILE *mailer)
{
	char *temp;
	mode_t prev_umask;
	priv_state priv;
	char *customSig;

	if ( mailer == NULL ) {
		return;
	}

	/* Want the letter to come from "condor" if possible */
	priv = set_condor_priv();

        customSig = NULL;
        if ((customSig = param("EMAIL_SIGNATURE")) != NULL) {
                fprintf( mailer, "\n\n");
               fprintf( mailer, "%s", customSig);
                fprintf( mailer, "\n");
                free(customSig);
        } else {
		
		/* Put a signature on the bottom of the email */
		fprintf( mailer, "\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n" );
		fprintf( mailer, "Questions about this message or Condor in general?\n" );

			/* See if there's an address users should use for help */
		temp = param( "CONDOR_SUPPORT_EMAIL" );
		if( ! temp ) {
			temp = param( "CONDOR_ADMIN" );
		}
		if( temp ) {
			fprintf( mailer, "Email address of the local Condor administrator: "
					 "%s\n", temp );
			free( temp );
		}
		fprintf( mailer, "The Official Condor Homepage is "
				 "http://www.cs.wisc.edu/condor\n" );
	}

	fflush(mailer);
	/* there are some oddities with how pclose can close a file. In some
		arches, pclose will create temp files for locking and they need to
		be of the correct perms in order to be deleted. So the umask is
		set to something useable for the close operation. -pete 9/11/99
	*/
	prev_umask = umask(022);
	/* 
	** we fclose() on UNIX, pclose on win32 
	*/
#if defined(WIN32)
	if (EMAIL_FINAL_COMMAND == NULL) {
		my_pclose( mailer );
	} else {
		char *email_filename = NULL;
		/* Should this be a pclose??? -Erik 9/21/00 */ 
		fclose( mailer );
		dprintf(D_FULLDEBUG,"Sending email via system(%s)\n",
			EMAIL_FINAL_COMMAND);
		system(EMAIL_FINAL_COMMAND);
		if ( (email_filename=strrchr(EMAIL_FINAL_COMMAND,'<')) ) {
			email_filename++;	/* go past the "<" */
			email_filename++;	/* go past the space after the < */
			if ( unlink(email_filename) == -1 ) {
				dprintf(D_ALWAYS,"email_close: cannot unlink temp file %s\n",
					email_filename);
			}
		}
		free(EMAIL_FINAL_COMMAND);
		EMAIL_FINAL_COMMAND = NULL;
	}
#else
	(void)fclose( mailer );
#endif
	umask(prev_umask);

	/* Set priv state back */
	set_priv(priv);

}
