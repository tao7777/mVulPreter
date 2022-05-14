FileTransfer::DoUpload(filesize_t *total_bytes, ReliSock *s)
{
	int rc;
	MyString fullname;
	filesize_t bytes;
	bool is_the_executable;
	bool upload_success = false;
	bool do_download_ack = false;
	bool do_upload_ack = false;
	bool try_again = false;
	int hold_code = 0;
	int hold_subcode = 0;
	MyString error_desc;
	bool I_go_ahead_always = false;
	bool peer_goes_ahead_always = false;
	DCTransferQueue xfer_queue(m_xfer_queue_contact_info);

	CondorError errstack;

	bool first_failed_file_transfer_happened = false;
	bool first_failed_upload_success = false;
	bool first_failed_try_again = false;
	int first_failed_hold_code = 0;
	int first_failed_hold_subcode = 0;
	MyString first_failed_error_desc;
	int first_failed_line_number;


	*total_bytes = 0;
	dprintf(D_FULLDEBUG,"entering FileTransfer::DoUpload\n");

	priv_state saved_priv = PRIV_UNKNOWN;
	if( want_priv_change ) {
		saved_priv = set_priv( desired_priv_state );
	}

	s->encode();

	if( !s->code(m_final_transfer_flag) ) {
		dprintf(D_FULLDEBUG,"DoUpload: exiting at %d\n",__LINE__);
		return_and_resetpriv( -1 );
	}
	if( !s->end_of_message() ) {
		dprintf(D_FULLDEBUG,"DoUpload: exiting at %d\n",__LINE__);
		return_and_resetpriv( -1 );
	}

	bool socket_default_crypto = s->get_encryption();

	if( want_priv_change && saved_priv == PRIV_UNKNOWN ) {
		saved_priv = set_priv( desired_priv_state );
	}

	FileTransferList filelist;
	ExpandFileTransferList( FilesToSend, filelist );

	FileTransferList::iterator filelist_it;
	for( filelist_it = filelist.begin();
		 filelist_it != filelist.end();
		 filelist_it++ )
	{
		char const *filename = filelist_it->srcName();
		char const *dest_dir = filelist_it->destDir();

		if( dest_dir && *dest_dir ) {
			dprintf(D_FULLDEBUG,"DoUpload: sending file %s to %s%c\n",filename,dest_dir,DIR_DELIM_CHAR);
		}
		else {
			dprintf(D_FULLDEBUG,"DoUpload: sending file %s\n",filename);
		}

		bool is_url;
		is_url = false;

		if( param_boolean("ENABLE_URL_TRANSFERS", true) && IsUrl(filename) ) {
			is_url = true;
			fullname = filename;
			dprintf(D_FULLDEBUG, "DoUpload: sending %s as URL.\n", filename);
		} else if( filename[0] != '/' && filename[0] != '\\' && filename[1] != ':' ){
			fullname.sprintf("%s%c%s",Iwd,DIR_DELIM_CHAR,filename);
		} else {
			fullname = filename;
		}

#ifdef WIN32
		if( !is_url && perm_obj && !is_the_executable &&
			(perm_obj->read_access(fullname.Value()) != 1) ) {
			upload_success = false;
			error_desc.sprintf("error reading from %s: permission denied",fullname.Value());
			do_upload_ack = true;    // tell receiver that we failed
			do_download_ack = true;
			try_again = false; // put job on hold
			hold_code = CONDOR_HOLD_CODE_UploadFileError;
			hold_subcode = EPERM;
			return ExitDoUpload(total_bytes,s,saved_priv,socket_default_crypto,
			                    upload_success,do_upload_ack,do_download_ack,
								try_again,hold_code,hold_subcode,
								error_desc.Value(),__LINE__);
		}
#endif
		if (is_the_executable) {} // Done to get rid of the compiler set-but-not-used warnings.




		int file_command = 1;
		int file_subcommand = 0;
		
		if ( DontEncryptFiles->file_contains_withwildcard(filename) ) {
			file_command = 3;
		}

		if ( EncryptFiles->file_contains_withwildcard(filename) ) {
			file_command = 2;
		}

		if ( X509UserProxy && file_strcmp( filename, X509UserProxy ) == 0 &&
			 DelegateX509Credentials ) {

			file_command = 4;
		}

		if ( is_url ) {
			file_command = 5;
		}

		if ( m_final_transfer_flag && OutputDestination ) {
			dprintf(D_FULLDEBUG, "FILETRANSFER: Using command 999:7 for OutputDestionation: %s\n",
					OutputDestination);

			file_command = 999;
			file_subcommand = 7;
		}

		bool fail_because_mkdir_not_supported = false;
		bool fail_because_symlink_not_supported = false;
		if( filelist_it->is_directory ) {
			if( filelist_it->is_symlink ) {
				fail_because_symlink_not_supported = true;
				dprintf(D_ALWAYS,"DoUpload: attempting to transfer symlink %s which points to a directory.  This is not supported.\n",filename);
			}
			else if( PeerUnderstandsMkdir ) {
				file_command = 6;
			}
			else {
				fail_because_mkdir_not_supported = true;
				dprintf(D_ALWAYS,"DoUpload: attempting to transfer directory %s, but the version of Condor we are talking to is too old to support that!\n",
						filename);
			}
		}

		dprintf ( D_FULLDEBUG, "FILETRANSFER: outgoing file_command is %i for %s\n",
				file_command, filename );

		if( !s->snd_int(file_command,FALSE) ) {
			dprintf(D_FULLDEBUG,"DoUpload: exiting at %d\n",__LINE__);
			return_and_resetpriv( -1 );
		}
		if( !s->end_of_message() ) {
			dprintf(D_FULLDEBUG,"DoUpload: exiting at %d\n",__LINE__);
			return_and_resetpriv( -1 );
		}

		if (file_command == 2) {
			s->set_crypto_mode(true);
		} else if (file_command == 3) {
			s->set_crypto_mode(false);
		}
		else {
			s->set_crypto_mode(socket_default_crypto);
		}

		MyString dest_filename;
		if ( ExecFile && !simple_init && (file_strcmp(ExecFile,filename)==0 )) {
			is_the_executable = true;
			dest_filename = CONDOR_EXEC;
		} else {
			is_the_executable = false;

			if( dest_dir && *dest_dir ) {
				dest_filename.sprintf("%s%c",dest_dir,DIR_DELIM_CHAR);
                        }
 
                       dest_filename.sprintf_cat( "%s", condor_basename(filename) );
                }
 
		if( !s->put(dest_filename.Value()) ) {
			dprintf(D_FULLDEBUG,"DoUpload: exiting at %d\n",__LINE__);
			return_and_resetpriv( -1 );
		}

		if( PeerDoesGoAhead ) {
			if( !s->end_of_message() ) {
				dprintf(D_FULLDEBUG, "DoUpload: failed on eom before GoAhead; exiting at %d\n",__LINE__);
				return_and_resetpriv( -1 );
			}

			if( !peer_goes_ahead_always ) {
				if( !ReceiveTransferGoAhead(s,fullname.Value(),false,peer_goes_ahead_always) ) {
					dprintf(D_FULLDEBUG, "DoUpload: exiting at %d\n",__LINE__);
					return_and_resetpriv( -1 );
				}
			}

			if( !I_go_ahead_always ) {
				if( !ObtainAndSendTransferGoAhead(xfer_queue,false,s,fullname.Value(),I_go_ahead_always) ) {
					dprintf(D_FULLDEBUG, "DoUpload: exiting at %d\n",__LINE__);
					return_and_resetpriv( -1 );
				}
			}

			s->encode();
		}

		if ( file_command == 999) {

			ClassAd file_info;
			file_info.Assign("ProtocolVersion", 1);
			file_info.Assign("Command", file_command);
			file_info.Assign("SubCommand", file_subcommand);


			if(file_subcommand == 7) {
				MyString source_filename;
				source_filename = Iwd;
				source_filename += DIR_DELIM_CHAR;
				source_filename += filename;

				MyString URL;
				URL = OutputDestination;
				URL += DIR_DELIM_CHAR;
				URL += filename;

				dprintf (D_FULLDEBUG, "DoUpload: calling IFTP(fn,U): fn\"%s\", U\"%s\"\n", source_filename.Value(), URL.Value());
				dprintf (D_FULLDEBUG, "LocalProxyName: %s\n", LocalProxyName.Value());
				rc = InvokeFileTransferPlugin(errstack, source_filename.Value(), URL.Value(), LocalProxyName.Value());
				dprintf (D_FULLDEBUG, "DoUpload: IFTP(fn,U): fn\"%s\", U\"%s\" returns %i\n", source_filename.Value(), URL.Value(), rc);

				file_info.Assign("Filename", source_filename);
				file_info.Assign("OutputDestination", URL);

				file_info.Assign("Result", rc);

				if (rc) {
					file_info.Assign("ErrorString", errstack.getFullText());
				}

				if(!file_info.put(*s)) {
					dprintf(D_FULLDEBUG,"DoDownload: exiting at %d\n",__LINE__);
					return_and_resetpriv( -1 );
				}

				MyString junkbuf;
				file_info.sPrint(junkbuf);
				bytes = junkbuf.Length();

			} else {
				dprintf( D_ALWAYS, "DoUpload: invalid subcommand %i, skipping %s.",
						file_subcommand, filename);
				bytes = 0;
				rc = 0;
			}
		} else if ( file_command == 4 ) {
			if ( (PeerDoesGoAhead || s->end_of_message()) ) {
				time_t expiration_time = GetDesiredDelegatedJobCredentialExpiration(&jobAd);
				rc = s->put_x509_delegation( &bytes, fullname.Value(), expiration_time, NULL );
				dprintf( D_FULLDEBUG,
				         "DoUpload: put_x509_delegation() returned %d\n",
				         rc );
			} else {
				rc = -1;
			}
		} else if (file_command == 5) {
			if(!s->code(fullname)) {
				dprintf( D_FULLDEBUG, "DoUpload: failed to send fullname: %s\n", fullname.Value());
				rc = -1;
			} else {
				dprintf( D_FULLDEBUG, "DoUpload: sent fullname and NO eom: %s\n", fullname.Value());
				rc = 0;
			}

			bytes = fullname.Length();

		} else if( file_command == 6 ) { // mkdir
			bytes = sizeof( filelist_it->file_mode );

			if( !s->put( filelist_it->file_mode ) ) {
				rc = -1;
				dprintf(D_ALWAYS,"DoUpload: failed to send mkdir mode\n");
			}
			else {
				rc = 0;
			}
		} else if( fail_because_mkdir_not_supported || fail_because_symlink_not_supported ) {
			if( TransferFilePermissions ) {
				rc = s->put_file_with_permissions( &bytes, NULL_FILE );
			}
			else {
				rc = s->put_file( &bytes, NULL_FILE );
			}
			if( rc == 0 ) {
				rc = PUT_FILE_OPEN_FAILED;
				errno = EISDIR;
			}
		} else if ( TransferFilePermissions ) {
			rc = s->put_file_with_permissions( &bytes, fullname.Value() );
		} else {
			rc = s->put_file( &bytes, fullname.Value() );
		}
		if( rc < 0 ) {
			int the_error = errno;
			upload_success = false;
			error_desc.sprintf("error sending %s",fullname.Value());
			if((rc == PUT_FILE_OPEN_FAILED) || (rc == PUT_FILE_PLUGIN_FAILED)) {
				if (rc == PUT_FILE_OPEN_FAILED) {

					error_desc.replaceString("sending","reading from");
					error_desc.sprintf_cat(": (errno %d) %s",the_error,strerror(the_error));
					if( fail_because_mkdir_not_supported ) {
						error_desc.sprintf_cat("; Remote condor version is too old to transfer directories.");
					}
					if( fail_because_symlink_not_supported ) {
						error_desc.sprintf_cat("; Transfer of symlinks to directories is not supported.");
					}
				} else {
					error_desc.sprintf_cat(": %s", errstack.getFullText());
				}
				try_again = false; // put job on hold
				hold_code = CONDOR_HOLD_CODE_UploadFileError;
				hold_subcode = the_error;


				if (first_failed_file_transfer_happened == false) {
					first_failed_file_transfer_happened = true;
					first_failed_upload_success = false;
					first_failed_try_again = false;
					first_failed_hold_code = hold_code;
					first_failed_hold_subcode = the_error;
					first_failed_error_desc = error_desc;
					first_failed_line_number = __LINE__;
				}
			}
			else {
				do_download_ack = true;
				do_upload_ack = false;
				try_again = true;

				return ExitDoUpload(total_bytes,s,saved_priv,
								socket_default_crypto,upload_success,
								do_upload_ack,do_download_ack,
			                    try_again,hold_code,hold_subcode,
			                    error_desc.Value(),__LINE__);
			}
		}

		if( !s->end_of_message() ) {
			dprintf(D_FULLDEBUG,"DoUpload: exiting at %d\n",__LINE__);
			return_and_resetpriv( -1 );
		}
		
		*total_bytes += bytes;


		if( dest_filename.FindChar(DIR_DELIM_CHAR) < 0 &&
			dest_filename != condor_basename(JobStdoutFile.Value()) &&
			dest_filename != condor_basename(JobStderrFile.Value()) )
		{
			Info.addSpooledFile( dest_filename.Value() );
		}
	}

	do_download_ack = true;
	do_upload_ack = true;

	if (first_failed_file_transfer_happened == true) {
		return ExitDoUpload(total_bytes,s,saved_priv,socket_default_crypto,
			first_failed_upload_success,do_upload_ack,do_download_ack,
			first_failed_try_again,first_failed_hold_code,
			first_failed_hold_subcode,first_failed_error_desc.Value(),
			first_failed_line_number);
	} 

	upload_success = true;
	return ExitDoUpload(total_bytes,s,saved_priv,socket_default_crypto,
	                    upload_success,do_upload_ack,do_download_ack,
	                    try_again,hold_code,hold_subcode,NULL,__LINE__);
}
