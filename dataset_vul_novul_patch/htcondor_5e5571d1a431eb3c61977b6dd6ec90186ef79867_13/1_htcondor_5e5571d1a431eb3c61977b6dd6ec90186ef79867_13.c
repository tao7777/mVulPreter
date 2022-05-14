display_sigset( const char *msg, sigset_t *mask )
{
	int					signo;
        NameTableIterator       next_sig( SigNames );
 
        if( msg ) {
               dprintf( D_ALWAYS, msg );
        }
        while( (signo = next_sig()) != -1 ) {
                if( sigismember(mask,signo) ) {
			dprintf( D_ALWAYS | D_NOHEADER, "%s ", SigNames.get_name(signo) );
		}
	}
	dprintf( D_ALWAYS | D_NOHEADER, "\n" );
}
