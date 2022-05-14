do_REMOTE_syscall()
{
	int condor_sysnum;
	int	rval = -1, result = -1, fd = -1, mode = -1, uid = -1, gid = -1;
	int length = -1;
	condor_errno_t terrno;
	char *path = NULL, *buffer = NULL;
	void *buf = NULL;

	syscall_sock->decode();

	dprintf(D_SYSCALLS, "About to decode condor_sysnum\n");

	rval = syscall_sock->code(condor_sysnum);
	if (!rval) {
		MyString err_msg;
		err_msg = "Can no longer talk to condor_starter <";
		err_msg += syscall_sock->peer_ip_str();
		err_msg += ':';
		err_msg += syscall_sock->peer_port();
		err_msg += '>';

		thisRemoteResource->closeClaimSock();

            /* It is possible that we are failing to read the
            syscall number because the starter went away
            because we *asked* it to go away. Don't be shocked
            and surprised if the startd/starter actually did
            what we asked when we deactivated the claim */
       if ( thisRemoteResource->wasClaimDeactivated() ) {
           return 0;
       }

		if( Shadow->supportsReconnect() ) {
			dprintf( D_ALWAYS, "%s\n", err_msg.Value() );

			const char* txt = "Socket between submit and execute hosts "
				"closed unexpectedly";
			Shadow->logDisconnectedEvent( txt ); 

			if (!Shadow->shouldAttemptReconnect(thisRemoteResource)) {
					dprintf(D_ALWAYS, "This job cannot reconnect to starter, so job exiting\n");
					Shadow->gracefulShutDown();
					EXCEPT( "%s", err_msg.Value() );
			}
			Shadow->reconnect();
			return 0;
		} else {
			EXCEPT( "%s", err_msg.Value() );
		}
	}

	dprintf(D_SYSCALLS,
		"Got request for syscall %s (%d)\n",
		shadow_syscall_name(condor_sysnum), condor_sysnum);

	switch( condor_sysnum ) {

	case CONDOR_register_starter_info:
	{
		ClassAd ad;
		result = ( ad.initFromStream(*syscall_sock) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_register_starter_info( &ad );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_register_job_info:
	{
		ClassAd ad;
		result = ( ad.initFromStream(*syscall_sock) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_register_job_info( &ad );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_get_job_info:
	{
		ClassAd *ad = NULL;
		bool delete_ad;

		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_get_job_info(ad, delete_ad);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		} else {
			result = ( ad->put(*syscall_sock) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		if ( delete_ad ) {
			delete ad;
		}
		return 0;
	}


	case CONDOR_get_user_info:
	{
		ClassAd *ad = NULL;

		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_get_user_info(ad);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		} else {
			result = ( ad->put(*syscall_sock) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}


	case CONDOR_job_exit:
	{
		int status=0;
		int reason=0;
		ClassAd ad;

		result = ( syscall_sock->code(status) );
		ASSERT( result );
		result = ( syscall_sock->code(reason) );
		ASSERT( result );
		result = ( ad.initFromStream(*syscall_sock) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_job_exit(status, reason, &ad);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return -1;
	}

	case CONDOR_job_termination:
	{
		ClassAd ad;
		result = ( ad.initFromStream(*syscall_sock) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_job_termination( &ad );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_begin_execution:
	{
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_begin_execution();
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_open:
	  {
		open_flags_t flags;
		int   lastarg;

		result = ( syscall_sock->code(flags) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  flags = %d\n", flags );
		result = ( syscall_sock->code(lastarg) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  lastarg = %d\n", lastarg );
		path = NULL;
		result = ( syscall_sock->code(path) );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );


		bool access_ok;
		if ( flags & O_RDONLY ) {
			access_ok = read_access(path);
		} else {
			access_ok = write_access(path);
		}

		errno = 0;
		if ( access_ok ) {
			rval = safe_open_wrapper_follow( path , flags , lastarg);
		} else {
			rval = -1;
			errno = EACCES;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}

		free( (char *)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_close:
	  {
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = close( fd);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_read:
	  {
		size_t   len;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(len) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  len = %ld\n", (long)len );
		buf = (void *)malloc( (unsigned)len );
		memset( buf, 0, (unsigned)len );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = read( fd , buf , len);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		if( rval >= 0 ) {
			result = ( syscall_sock->code_bytes_bool(buf, rval) );
			ASSERT( result );
		}
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_write:
	  {
		size_t   len;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(len) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  len = %ld\n", (long)len );
		buf = (void *)malloc( (unsigned)len );
		memset( buf, 0, (unsigned)len );
		result = ( syscall_sock->code_bytes_bool(buf, len) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = write( fd , buf , len);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_lseek:
	case CONDOR_lseek64:
	case CONDOR_llseek:
	  {
		off_t   offset;
		int   whence;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(offset) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  offset = %ld\n", (long)offset );
		result = ( syscall_sock->code(whence) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  whence = %d\n", whence );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = lseek( fd , offset , whence);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_unlink:
	  {
		path = NULL;
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		if ( write_access(path) ) {
			errno = 0;
			rval = unlink( path);
		} else {
			rval = -1;
			errno = EACCES;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char *)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_rename:
	  {
		char *  from;
		char *  to;

		to = NULL;
		from = NULL;
		result = ( syscall_sock->code(from) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  from = %s\n", from );
		result = ( syscall_sock->code(to) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  to = %s\n", to );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		if ( write_access(from) && write_access(to) ) {
			errno = 0;
			rval = rename( from , to);
		} else {
			rval = -1;
			errno = EACCES;
		}

		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char *)to );
		free( (char *)from );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_register_mpi_master_info:
	{
		ClassAd ad;
		result = ( ad.initFromStream(*syscall_sock) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pseudo_register_mpi_master_info( &ad );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_mkdir:
	  {
		path = NULL;
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(mode) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  mode = %d\n", mode );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		if ( write_access(path) ) {
			errno = 0;
			rval = mkdir(path,mode);
		} else {
			rval = -1;
			errno = EACCES;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char *)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_rmdir:
	  {
		path = NULL;
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		if ( write_access(path) ) {
			errno = 0;
			rval = rmdir( path);
		} else {
			rval = -1;
			errno = EACCES;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_fsync:
	  {
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fs = %d\n", fd );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = fsync(fd);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}

	case CONDOR_get_file_info_new:
	  {
		char *  logical_name;
		char *  actual_url;
 
		actual_url = NULL;
		logical_name = NULL;
		ASSERT( syscall_sock->code(logical_name) );
		ASSERT( syscall_sock->end_of_message() );;
 
		errno = (condor_errno_t)0;
		rval = pseudo_get_file_info_new( logical_name , actual_url );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, (int)terrno );
 
		syscall_sock->encode();
		ASSERT( syscall_sock->code(rval) );
		if( rval < 0 ) {
			ASSERT( syscall_sock->code(terrno) );
		}
		if( rval >= 0 ) {
			ASSERT( syscall_sock->code(actual_url) );
		}
		free( (char *)actual_url );
		free( (char *)logical_name );
		ASSERT( syscall_sock->end_of_message() );;
		return 0;
	}

	case CONDOR_ulog:
	{
		ClassAd ad;

		result = ( ad.initFromStream(*syscall_sock) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		rval = pseudo_ulog(&ad);
		dprintf( D_SYSCALLS, "\trval = %d\n", rval );


		return 0;
	}

	case CONDOR_get_job_attr:
	  {
		char *  attrname = 0;

		assert( syscall_sock->code(attrname) );
		assert( syscall_sock->end_of_message() );;

		errno = (condor_errno_t)0;
		MyString expr;
		if ( thisRemoteResource->allowRemoteReadAttributeAccess(attrname) ) {
			rval = pseudo_get_job_attr( attrname , expr);
			terrno = (condor_errno_t)errno;
		} else {
			rval = -1;
			terrno = (condor_errno_t)EACCES;
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, (int)terrno );

		syscall_sock->encode();
		assert( syscall_sock->code(rval) );
		if( rval < 0 ) {
			assert( syscall_sock->code(terrno) );
		}
		if( rval >= 0 ) {
			assert( syscall_sock->put(expr.Value()) );
		}
		free( (char *)attrname );
		assert( syscall_sock->end_of_message() );;
		return 0;
	}

	case CONDOR_set_job_attr:
	  {
		char *  attrname = 0;
		char *  expr = 0;

		assert( syscall_sock->code(expr) );
		assert( syscall_sock->code(attrname) );
		assert( syscall_sock->end_of_message() );;

		errno = (condor_errno_t)0;
		if ( thisRemoteResource->allowRemoteWriteAttributeAccess(attrname) ) {
			rval = pseudo_set_job_attr( attrname , expr , true );
			terrno = (condor_errno_t)errno;
		} else {
			rval = -1;
			terrno = (condor_errno_t)EACCES;
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, (int)terrno );

		syscall_sock->encode();
		assert( syscall_sock->code(rval) );
		if( rval < 0 ) {
			assert( syscall_sock->code(terrno) );
		}
		free( (char *)expr );
		free( (char *)attrname );
		assert( syscall_sock->end_of_message() );;
		return 0;
	}

	case CONDOR_constrain:
	  {
		char *  expr = 0;

		assert( syscall_sock->code(expr) );
		assert( syscall_sock->end_of_message() );;

		errno = (condor_errno_t)0;
		if ( thisRemoteResource->allowRemoteWriteAttributeAccess(ATTR_REQUIREMENTS) ) {
			rval = pseudo_constrain( expr);
			terrno = (condor_errno_t)errno;
		} else {
			rval = -1;
			terrno = (condor_errno_t)EACCES;
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, (int)terrno );

		syscall_sock->encode();
		assert( syscall_sock->code(rval) );
		if( rval < 0 ) {
			assert( syscall_sock->code(terrno) );
		}
		free( (char *)expr );
		assert( syscall_sock->end_of_message() );;
		return 0;
	}
	case CONDOR_get_sec_session_info:
	{
		MyString starter_reconnect_session_info;
		MyString starter_filetrans_session_info;
		MyString reconnect_session_id;
		MyString reconnect_session_info;
		MyString reconnect_session_key;
		MyString filetrans_session_id;
		MyString filetrans_session_info;
		MyString filetrans_session_key;
		bool socket_default_crypto = syscall_sock->get_encryption();
		if( !socket_default_crypto ) {
			syscall_sock->set_crypto_mode(true);
		}
		assert( syscall_sock->code(starter_reconnect_session_info) );
		assert( syscall_sock->code(starter_filetrans_session_info) );
		assert( syscall_sock->end_of_message() );

		errno = (condor_errno_t)0;
		rval = pseudo_get_sec_session_info(
			starter_reconnect_session_info.Value(),
			reconnect_session_id,
			reconnect_session_info,
			reconnect_session_key,
			starter_filetrans_session_info.Value(),
			filetrans_session_id,
			filetrans_session_info,
			filetrans_session_key );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, (int)terrno );

		syscall_sock->encode();
		assert( syscall_sock->code(rval) );
		if( rval < 0 ) {
			assert( syscall_sock->code(terrno) );
		}
		else {
			assert( syscall_sock->code(reconnect_session_id) );
			assert( syscall_sock->code(reconnect_session_info) );
			assert( syscall_sock->code(reconnect_session_key) );

			assert( syscall_sock->code(filetrans_session_id) );
			assert( syscall_sock->code(filetrans_session_info) );
			assert( syscall_sock->code(filetrans_session_key) );
		}

		assert( syscall_sock->end_of_message() );

		if( !socket_default_crypto ) {
			syscall_sock->set_crypto_mode( false );  // restore default
		}
		return 0;
	}
#ifdef WIN32
#else
	case CONDOR_pread:
	  {
		size_t len, offset;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(len) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  len = %ld\n", (long)len );
		result = ( syscall_sock->code(offset) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  offset = %ld\n", (long)offset );
		buf = (void *)malloc( (unsigned)len );
		memset( buf, 0, (unsigned)len );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pread( fd , buf , len, offset );
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(buf, rval) );
			ASSERT( result );
		}
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_pwrite:
	  {
		size_t   len, offset;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(len) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  len = %ld\n", (long)len );
		result = ( syscall_sock->code(offset) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  offset = %ld\n", (long)offset);
		buf = malloc( (unsigned)len );
		memset( buf, 0, (unsigned)len );
		result = ( syscall_sock->code_bytes_bool(buf, len) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = pwrite( fd , buf , len, offset);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_sread:
	  {
		size_t   len, offset, stride_length, stride_skip;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(len) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  len = %ld\n", (long)len );
		result = ( syscall_sock->code(offset) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  offset = %ld\n", (long)offset );
		result = ( syscall_sock->code(stride_length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  stride_length = %ld\n", (long)stride_length);
		result = ( syscall_sock->code(stride_skip) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  stride_skip = %ld\n", (long)stride_skip);
		buf = (void *)malloc( (unsigned)len );
		memset( buf, 0, (unsigned)len );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = EINVAL;
		rval = -1;
		unsigned int total = 0;
		buffer = (char*)buf;

		while(total < len && stride_length > 0) {
			if(len - total < stride_length) {
				stride_length = len - total;
			}
			rval = pread( fd, (void*)&buffer[total], stride_length, offset );
			if(rval >= 0) {
				total += rval;
				offset += stride_skip;
			}
			else {
				break;
			}
		}

		syscall_sock->encode();
		if( rval < 0 ) {
			result = ( syscall_sock->code(rval) );
			ASSERT( result );
			terrno = (condor_errno_t)errno;
			dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", total, terrno );
			result = ( syscall_sock->code(total) );
			ASSERT( result );
			dprintf( D_ALWAYS, "buffer: %s\n", buffer);
			result = ( syscall_sock->code_bytes_bool(buf, total) );
			ASSERT( result );
		}
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_swrite:
	  {
		size_t   len, offset, stride_length, stride_skip;

		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(len) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  len = %ld\n", (long)len );
		result = ( syscall_sock->code(offset) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  offset = %ld\n", (long)offset);
		result = ( syscall_sock->code(stride_length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  stride_length = %ld\n", (long)stride_length);
		result = ( syscall_sock->code(stride_skip) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  stride_skip = %ld\n", (long)stride_skip);
		buf = (void *)malloc( (unsigned)len );
		memset( buf, 0, (unsigned)len );
		result = ( syscall_sock->code_bytes_bool(buf, len) );
		ASSERT( result );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = EINVAL;
		rval = -1;
		unsigned int total = 0;
		buffer = (char*)buf;

		while(total < len && stride_length > 0) {
			if(len - total < stride_length) {
				stride_length = len - total;
			}
			rval = pwrite( fd, (void*)&buffer[total], stride_length, offset);
			if(rval >= 0) {
				total += rval;
				offset += stride_skip;
			}
			else {
				break;
			}
		}
		
		syscall_sock->encode();
		if( rval < 0 ) {
			terrno = (condor_errno_t)errno;
			dprintf( D_SYSCALLS, "\trval = %d, errno = %d (%s)\n", rval, terrno, strerror(errno));
			result = ( syscall_sock->code(rval) );
			ASSERT( result );
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			dprintf( D_SYSCALLS, "\trval = %d, errno = %d (%s)\n", total, terrno, strerror(errno));
			result = ( syscall_sock->code(total) );
			ASSERT( result );
		}
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_rmall:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		if ( write_access(path) ) {
			rval = rmdir(path);
			
			if(rval == -1) {
				Directory dir(path);
				if(dir.Remove_Entire_Directory()) {
					rval = rmdir(path);
				}
			}
		} else {
			rval = -1;
			errno = EACCES;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char *)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
case CONDOR_getfile:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		fd = safe_open_wrapper_follow( path, O_RDONLY );
		if(fd >= 0) {
			struct stat info;
			stat(path, &info);
			length = info.st_size;
			buf = (void *)malloc( (unsigned)length );
			memset( buf, 0, (unsigned)length );

			errno = 0;
			rval = read( fd , buf , length);
		} else {
			rval = fd;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {	
			result = ( syscall_sock->code_bytes_bool(buf, rval) );
			ASSERT( result );
		}
		free( (char *)path );
		free( buf );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
case CONDOR_putfile:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf(D_SYSCALLS, "  path: %s\n", path);
		result = ( syscall_sock->code(mode) );
		ASSERT( result );
		dprintf(D_SYSCALLS, "  mode: %d\n", mode);
		result = ( syscall_sock->code(length) );
		ASSERT( result );
		dprintf(D_SYSCALLS, "  length: %d\n", length);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		fd = safe_open_wrapper_follow(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		if( fd < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		int num = -1;
		if(fd >= 0) {
			syscall_sock->decode();
			buffer = (char*)malloc( (unsigned)length );
			memset( buffer, 0, (unsigned)length );
			result = ( syscall_sock->code_bytes_bool(buffer, length) );
			ASSERT( result );
			result = ( syscall_sock->end_of_message() );
			ASSERT( result );
			num = write(fd, buffer, length);
		}
		else {
			dprintf(D_SYSCALLS, "Unable to put file %s\n", path);
		}
		close(fd);
		
		syscall_sock->encode();
		result = ( syscall_sock->code(num) );
		ASSERT( result );
		if( num < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free((char*)path);
		free((char*)buffer);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
case CONDOR_getlongdir:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = -1;
		MyString msg, check;
		const char *next;
		Directory directory(path);
		struct stat stat_buf;
		char line[1024];
		
		while((next = directory.Next())) {
			dprintf(D_ALWAYS, "next: %s\n", next);
			msg.sprintf_cat("%s\n", next);
			check.sprintf("%s%c%s", path, DIR_DELIM_CHAR, next);
			rval = stat(check.Value(), &stat_buf);
			terrno = (condor_errno_t)errno;
			if(rval == -1) {
				break;
			}
			if(stat_string(line, &stat_buf) < 0) {
				rval = -1;
				break;
			}
			msg.sprintf_cat("%s", line);
		}
		terrno = (condor_errno_t)errno;
		if(msg.Length() > 0) {
			msg.sprintf_cat("\n");	// Needed to signify end of data
			rval = msg.Length();
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->put(msg.Value()) );
			ASSERT( result );
		}
		free((char*)path);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
case CONDOR_getdir:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		rval = -1;
		MyString msg;
		const char *next;
		Directory directory(path);
 
                while((next = directory.Next())) {
                       msg.sprintf_cat("%s", next);
                        msg.sprintf_cat("\n");
                }
                terrno = (condor_errno_t)errno;
		if(msg.Length() > 0) {
			msg.sprintf_cat("\n");	// Needed to signify end of data
			rval = msg.Length();
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->put(msg.Value()) );
			ASSERT( result );
		}
		free((char*)path);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_whoami:
	{
		result = ( syscall_sock->code(length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  length = %d\n", length );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		buffer = (char*)malloc( (unsigned)length );
		int size = 6;
		if(length < size) {
			rval = -1;
			terrno = (condor_errno_t) ENOSPC;
		}
		else {
			rval = sprintf(buffer, "CONDOR");
			terrno = (condor_errno_t) errno;
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval != size) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(buffer, rval));
			ASSERT( result );
		}
		free((char*)buffer);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_whoareyou:
	{
		char *host = NULL;

		result = ( syscall_sock->code(host) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  host = %s\n", host );
		result = ( syscall_sock->code(length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  length = %d\n", length );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		buffer = (char*)malloc( (unsigned)length );
		int size = 7;
		if(length < size) {
			rval = -1;
			terrno = (condor_errno_t) ENOSPC;
		}
		else {
			rval = sprintf(buffer, "UNKNOWN");
			terrno = (condor_errno_t) errno;
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval != size) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(buffer, rval));
			ASSERT( result );
		}
		free((char*)buffer);
		free((char*)host);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_fstatfs:
	{
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
#if defined(Solaris)
		struct statvfs statfs_buf;
		rval = fstatvfs(fd, &statfs_buf);
#else
		struct statfs statfs_buf;
		rval = fstatfs(fd, &statfs_buf);
#endif
		terrno = (condor_errno_t)errno;
		char line[1024];
		if(rval == 0) {
			if(statfs_string(line, &statfs_buf) < 0) {
				rval = -1;
				terrno = (condor_errno_t)errno;
			}
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(line, 1024) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;	
	}
	case CONDOR_fchown:
	{
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(uid) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  uid = %d\n", uid );
		result = ( syscall_sock->code(gid) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  gid = %d\n", gid );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = fchown(fd, uid, gid);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_fchmod:
	{
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(mode) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  mode = %d\n", mode );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = fchmod(fd, (mode_t)mode);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_ftruncate:
	{
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->code(length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  length = %d\n", length );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = ftruncate(fd, length);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	
	
	
	
	
	
	case CONDOR_link:
	{
		char *newpath = NULL;

		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(newpath) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  newpath = %s\n", newpath );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = link(path, newpath);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free((char*)path);
		free((char*)newpath);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_symlink:
	{
		char *newpath = NULL;

		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(newpath) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  newpath = %s\n", newpath );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = symlink(path, newpath);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free((char*)path);
		free((char*)newpath);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_readlink:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  length = %d\n", length );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		char *lbuffer = (char*)malloc(length);
		errno = 0;
		rval = readlink(path, lbuffer, length);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(lbuffer, rval));
			ASSERT( result );
		}
		free(lbuffer);
		free(path);
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_lstat:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		struct stat stat_buf;
		rval = lstat(path, &stat_buf);
		terrno = (condor_errno_t)errno;
		char line[1024];
		if(rval == 0) {
			if(stat_string(line, &stat_buf) < 0) {
				rval = -1;
				terrno = (condor_errno_t)errno;
			}
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(line, 1024) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_statfs:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
#if defined(Solaris)
		struct statvfs statfs_buf;
		rval = statvfs(path, &statfs_buf);
#else
		struct statfs statfs_buf;
		rval = statfs(path, &statfs_buf);
#endif
		terrno = (condor_errno_t)errno;
		char line[1024];
		if(rval == 0) {
			if(statfs_string(line, &statfs_buf) < 0) {
				rval = -1;
				terrno = (condor_errno_t)errno;
			}
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(line, 1024) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_chown:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(uid) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  uid = %d\n", uid );
		result = ( syscall_sock->code(gid) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  gid = %d\n", gid );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = chown(path, uid, gid);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
	
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_lchown:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(uid) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  uid = %d\n", uid );
		result = ( syscall_sock->code(gid) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  gid = %d\n", gid );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = lchown(path, uid, gid);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
	
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_truncate:
	{
		
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(length) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  length = %d\n", length );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		if ( write_access(path) ) {
			errno = 0;
			rval = truncate(path, length);
		} else {
			rval = -1;
			errno = EACCES;
		}
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
#endif // ! WIN32

	case CONDOR_fstat:
	{
		result = ( syscall_sock->code(fd) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  fd = %d\n", fd );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		struct stat stat_buf;
		rval = fstat(fd, &stat_buf);
		terrno = (condor_errno_t)errno;
		char line[1024];
		if(rval == 0) {
			if(stat_string(line, &stat_buf) < 0) {
				rval = -1;
				terrno = (condor_errno_t)errno;
			}
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(line, 1024) );
			ASSERT( result );
		}
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;	
	}
	case CONDOR_stat:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );

		errno = 0;
		struct stat stat_buf;
		rval = stat(path, &stat_buf);
		terrno = (condor_errno_t)errno;
		char line[1024];
		if(rval == 0) {
			if(stat_string(line, &stat_buf) < 0) {
				rval = -1;
				terrno = (condor_errno_t)errno;
			}
		}
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if( rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		else {
			result = ( syscall_sock->code_bytes_bool(line, 1024) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_access:
	{
		int flags = -1;

		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(flags) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  flags = %d\n", flags );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = access(path, flags);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0 ) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_chmod:
	{
		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(mode) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  mode = %d\n", mode );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		errno = 0;
		rval = chmod(path, mode);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );
		
		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	case CONDOR_utime:
	{
		time_t actime = -1, modtime = -1;

		result = ( syscall_sock->code(path) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  path = %s\n", path );
		result = ( syscall_sock->code(actime) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  actime = %ld\n", actime );
		result = ( syscall_sock->code(modtime) );
		ASSERT( result );
		dprintf( D_SYSCALLS, "  modtime = %ld\n", modtime );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		
		struct utimbuf ut;
		ut.actime = actime;
		ut.modtime = modtime;
		
		errno = 0;
		rval = utime(path, &ut);
		terrno = (condor_errno_t)errno;
		dprintf( D_SYSCALLS, "\trval = %d, errno = %d\n", rval, terrno );

		syscall_sock->encode();
		result = ( syscall_sock->code(rval) );
		ASSERT( result );
		if(rval < 0) {
			result = ( syscall_sock->code( terrno ) );
			ASSERT( result );
		}
		free( (char*)path );
		result = ( syscall_sock->end_of_message() );
		ASSERT( result );
		return 0;
	}
	default:
	{
		dprintf(D_ALWAYS, "ERROR: unknown syscall %d received\n", condor_sysnum );
		return 0;
		
	}

	}	/* End of switch on system call number */

	return -1;

}	/* End of do_REMOTE_syscall() procedure */
