ReadUserLogState::SetState( const ReadUserLog::FileState &state )
{
	const ReadUserLogFileState::FileState *istate;
	if ( !convertState(state, istate) ) {
		return false;
	}

	if ( strcmp( istate->m_signature, FileStateSignature ) ) {
		m_init_error = true;
		return false;
	}
	if ( istate->m_version != FILESTATE_VERSION ) {
		m_init_error = true;
		return false;
	}

	m_base_path = istate->m_base_path;

	m_max_rotations = istate->m_max_rotations;
	Rotation( istate->m_rotation, false, true );

	m_log_type = istate->m_log_type;
	m_uniq_id  = istate->m_uniq_id;
	m_sequence = istate->m_sequence;

	m_stat_buf.st_ino   = istate->m_inode;
	m_stat_buf.st_ctime = istate->m_ctime;
	m_stat_buf.st_size  = istate->m_size.asint;
	m_stat_valid = true;

	m_offset = istate->m_offset.asint;
	m_event_num = istate->m_event_num.asint;

	m_log_position = istate->m_log_position.asint;
	m_log_record   = istate->m_log_record.asint;

	m_update_time  = istate->m_update_time;

	m_initialized  = true;
 
        MyString        str;
        GetStateString( str, "Restored reader state" );
       dprintf( D_FULLDEBUG, "%s", str.Value() );
 
        return true;
 }
