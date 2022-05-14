produce_output()
{
	char	*str;
	FILE	*mailer;
	MyString subject,szTmp;
	subject.sprintf("condor_preen results %s: %d old file%s found", 
		my_full_hostname(), BadFiles->number(), 
		(BadFiles->number() > 1)?"s":"");

	if( MailFlag ) {
		if( (mailer=email_open(PreenAdmin, subject.Value())) == NULL ) {
			EXCEPT( "Can't do email_open(\"%s\", \"%s\")\n",PreenAdmin,subject.Value());
		}
	} else {
		mailer = stdout;
        }
 
        szTmp.sprintf("The condor_preen process has found the following stale condor files on <%s>:\n\n",  get_local_hostname().Value());
       dprintf(D_ALWAYS, szTmp.Value()); 
                
        if( MailFlag ) {
                fprintf( mailer, "\n" );
               fprintf( mailer, szTmp.Value());
        }
 
        for( BadFiles->rewind(); (str = BadFiles->next()); ) {
                szTmp.sprintf("  %s\n", str);
               dprintf(D_ALWAYS, szTmp.Value() );
               fprintf( mailer, szTmp.Value() );
        }
 
        if( MailFlag ) {
		const char *explanation = "\n\nWhat is condor_preen?\n\n"
"The condor_preen tool examines the directories belonging to Condor, and\n"
"removes extraneous files and directories which may be left over from Condor\n"
"processes which terminated abnormally either due to internal errors or a\n"
"system crash.  The directories checked are the LOG, EXECUTE, and SPOOL\n"
"directories as defined in the Condor configuration files.  The condor_preen\n"
"tool is intended to be run as user root (or user condor) periodically as a\n"
"backup method to ensure reasonable file system cleanliness in the face of\n"
"errors. This is done automatically by default by the condor_master daemon.\n"
"It may also be explicitly invoked on an as needed basis.\n\n"
"See the Condor manual section on condor_preen for more details.\n";

		fprintf( mailer, "%s\n", explanation );
		email_close( mailer );
	}
}
