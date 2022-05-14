sysapi_translate_arch( const char *machine, const char *)
{
	char tmp[64];
	char *tmparch;

#if defined(AIX)
	/* AIX machines have a ton of different models encoded into the uname
		structure, so go to some other function to decode and group the
		architecture together */
	struct utsname buf;

	if( uname(&buf) < 0 ) {
		return NULL;
	}

	return( get_aix_arch( &buf ) );

#elif defined(HPUX)

	return( get_hpux_arch( ) );
#else

	if( !strcmp(machine, "alpha") ) {
		sprintf( tmp, "ALPHA" );
	}
	else if( !strcmp(machine, "i86pc") ) {
		sprintf( tmp, "INTEL" );
	}
	else if( !strcmp(machine, "i686") ) {
		sprintf( tmp, "INTEL" );
	}
	else if( !strcmp(machine, "i586") ) {
		sprintf( tmp, "INTEL" );
	}
	else if( !strcmp(machine, "i486") ) {
		sprintf( tmp, "INTEL" );
	}
	else if( !strcmp(machine, "i386") ) { //LDAP entry
#if defined(Darwin)
		/* Mac OS X often claims to be i386 in uname, even if the
		 * hardware is x86_64 and the OS can run 64-bit binaries.
		 * We'll base our architecture name on the default build
		 * target for gcc. In 10.5 and earlier, that's i386.
		 * On 10.6, it's x86_64.
		 * The value we're querying is the kernel version.
		 * 10.6 kernels have a version that starts with "10."
		 * Older versions have a lower first number.
		 */
		int ret;
		char val[32];
		size_t len = sizeof(val);

		/* assume x86 */
		sprintf( tmp, "INTEL" );
		ret = sysctlbyname("kern.osrelease", &val, &len, NULL, 0);
		if (ret == 0 && strncmp(val, "10.", 3) == 0) {
			/* but we could be proven wrong */
			sprintf( tmp, "X86_64" );
		}
#else
		sprintf( tmp, "INTEL" );
#endif
	}
	else if( !strcmp(machine, "ia64") ) {
		sprintf( tmp, "IA64" );
	}
	else if( !strcmp(machine, "x86_64") ) {
		sprintf( tmp, "X86_64" );
	}
	else if( !strcmp(machine, "amd64") ) {
		sprintf( tmp, "X86_64" );
	}
	else if( !strcmp(machine, "sun4u") ) {
		sprintf( tmp, "SUN4u" );
	}
	else if( !strcmp(machine, "sun4m") ) {
		sprintf( tmp, "SUN4x" );
	}
	else if( !strcmp(machine, "sun4c") ) {
		sprintf( tmp, "SUN4x" );
	}
	else if( !strcmp(machine, "sparc") ) { //LDAP entry
		sprintf( tmp, "SUN4x" );
	}
	else if( !strcmp(machine, "Power Macintosh") ) { //LDAP entry
		sprintf( tmp, "PPC" );
	}
	else if( !strcmp(machine, "ppc") ) {
		sprintf( tmp, "PPC" );
	}
	else if( !strcmp(machine, "ppc32") ) {
		sprintf( tmp, "PPC" );
	}
	else if( !strcmp(machine, "ppc64") ) {
		sprintf( tmp, "PPC64" );
        }
        else {
               sprintf( tmp, "%s", machine );
        }
 
        tmparch = strdup( tmp );
	if( !tmparch ) {
		EXCEPT( "Out of memory!" );
	}
	return( tmparch );
#endif /* if HPUX else */
}
