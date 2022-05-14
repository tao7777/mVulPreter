 usage(int iExitCode)
 {
        char word[32];
       sprintf( word, "%s", getJobActionString(mode) );
        fprintf( stderr, "Usage: %s [options] [constraints]\n", MyName );
        fprintf( stderr, " where [options] is zero or more of:\n" );
        fprintf( stderr, "  -help               Display this message and exit\n" );
	fprintf( stderr, "  -version            Display version information and exit\n" );


	fprintf( stderr, "  -name schedd_name   Connect to the given schedd\n" );
	fprintf( stderr, "  -pool hostname      Use the given central manager to find daemons\n" );
	fprintf( stderr, "  -addr <ip:port>     Connect directly to the given \"sinful string\"\n" );
	if( mode == JA_REMOVE_JOBS || mode == JA_REMOVE_X_JOBS ) {
		fprintf( stderr, "  -reason reason      Use the given RemoveReason\n");
	} else if( mode == JA_RELEASE_JOBS ) {
		fprintf( stderr, "  -reason reason      Use the given ReleaseReason\n");
	} else if( mode == JA_HOLD_JOBS ) {
		fprintf( stderr, "  -reason reason      Use the given HoldReason\n");
		fprintf( stderr, "  -subcode number     Set HoldReasonSubCode\n");
	}

	if( mode == JA_REMOVE_JOBS || mode == JA_REMOVE_X_JOBS ) {
		fprintf( stderr,
				     "  -forcex             Force the immediate local removal of jobs in the X state\n"
		         "                      (only affects jobs already being removed)\n" );
	}
	if( mode == JA_VACATE_JOBS || mode == JA_VACATE_FAST_JOBS ) {
		fprintf( stderr,
				     "  -fast               Use a fast vacate (hardkill)\n" );
	}
	fprintf( stderr, " and where [constraints] is one of:\n" );
	fprintf( stderr, "  cluster.proc        %s the given job\n", word );
	fprintf( stderr, "  cluster             %s the given cluster of jobs\n", word );
	fprintf( stderr, "  user                %s all jobs owned by user\n", word );
	fprintf( stderr, "  -constraint expr    %s all jobs matching the boolean expression\n", word );
	fprintf( stderr, "  -all                %s all jobs "
			 "(cannot be used with other constraints)\n", word );
	exit( iExitCode );
}
