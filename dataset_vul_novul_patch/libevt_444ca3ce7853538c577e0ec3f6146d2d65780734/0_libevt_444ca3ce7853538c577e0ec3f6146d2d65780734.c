int libevt_record_values_read_event(
     libevt_record_values_t *record_values,
     uint8_t *record_data,
     size_t record_data_size,
     uint8_t strict_mode,
     libcerror_error_t **error )
{
	static char *function      = "libevt_record_values_read_event";
	size_t record_data_offset  = 0;
	size_t strings_data_offset = 0;
	ssize_t value_data_size    = 0;
	uint32_t data_offset       = 0;
	uint32_t data_size         = 0;
	uint32_t members_data_size = 0;
	uint32_t size              = 0;
	uint32_t size_copy         = 0;
	uint32_t strings_offset    = 0;
	uint32_t strings_size      = 0;
	uint32_t user_sid_offset   = 0;
	uint32_t user_sid_size     = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t value_32bit       = 0;
	uint16_t value_16bit       = 0;
#endif

	if( record_values == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record values.",
		 function );

		return( -1 );
	}
	if( record_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid record data.",
		 function );

		return( -1 );
	}
	if( record_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid record data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( record_data_size < ( sizeof( evt_record_event_header_t ) + 4 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: record data size value out of bounds.",
		 function );

		return( -1 );
	}
	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->size,
	 size );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->record_number,
	 record_values->number );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->creation_time,
	 record_values->creation_time );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->written_time,
	 record_values->written_time );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->event_identifier,
	 record_values->event_identifier );

	byte_stream_copy_to_uint16_little_endian(
	 ( (evt_record_event_header_t *) record_data )->event_type,
	 record_values->event_type );

	byte_stream_copy_to_uint16_little_endian(
	 ( (evt_record_event_header_t *) record_data )->event_category,
	 record_values->event_category );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->strings_offset,
	 strings_offset );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->user_sid_size,
	 user_sid_size );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->user_sid_offset,
	 user_sid_offset );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->data_size,
	 data_size );

	byte_stream_copy_to_uint32_little_endian(
	 ( (evt_record_event_header_t *) record_data )->data_offset,
	 data_offset );

	byte_stream_copy_to_uint32_little_endian(
	 &( record_data[ record_data_size - 4 ] ),
	 size_copy );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: size\t\t\t\t\t: %" PRIu32 "\n",
		 function,
		 size );

		libcnotify_printf(
		 "%s: signature\t\t\t\t: %c%c%c%c\n",
		 function,
		 ( (evt_record_event_header_t *) record_data )->signature[ 0 ],
		 ( (evt_record_event_header_t *) record_data )->signature[ 1 ],
		 ( (evt_record_event_header_t *) record_data )->signature[ 2 ],
		 ( (evt_record_event_header_t *) record_data )->signature[ 3 ] );

		libcnotify_printf(
		 "%s: record number\t\t\t\t: %" PRIu32 "\n",
		 function,
		 record_values->number );

		if( libevt_debug_print_posix_time_value(
		     function,
		     "creation time\t\t\t\t",
		     ( (evt_record_event_header_t *) record_data )->creation_time,
		     4,
		     LIBFDATETIME_ENDIAN_LITTLE,
		     LIBFDATETIME_POSIX_TIME_VALUE_TYPE_SECONDS_32BIT_SIGNED,
		     LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print POSIX time value.",
			 function );

			goto on_error;
		}
		if( libevt_debug_print_posix_time_value(
		     function,
		     "written time\t\t\t\t",
		     ( (evt_record_event_header_t *) record_data )->written_time,
		     4,
		     LIBFDATETIME_ENDIAN_LITTLE,
		     LIBFDATETIME_POSIX_TIME_VALUE_TYPE_SECONDS_32BIT_SIGNED,
		     LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print POSIX time value.",
			 function );

			goto on_error;
		}
		libcnotify_printf(
		 "%s: event identifier\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 record_values->event_identifier );

		libcnotify_printf(
		 "%s: event identifier: code\t\t\t: %" PRIu32 "\n",
		 function,
		 record_values->event_identifier & 0x0000ffffUL );

		libcnotify_printf(
		 "%s: event identifier: facility\t\t: %" PRIu32 "\n",
		 function,
		 ( record_values->event_identifier & 0x0fff0000UL ) >> 16 );

		libcnotify_printf(
		 "%s: event identifier: reserved\t\t: %" PRIu32 "\n",
		 function,
		 ( record_values->event_identifier & 0x10000000UL ) >> 28 );

		libcnotify_printf(
		 "%s: event identifier: customer flags\t: %" PRIu32 "\n",
		 function,
		 ( record_values->event_identifier & 0x20000000UL ) >> 29 );

		libcnotify_printf(
		 "%s: event identifier: severity\t\t: %" PRIu32 " (",
		 function,
		 ( record_values->event_identifier & 0xc0000000UL ) >> 30 );
		libevt_debug_print_event_identifier_severity(
		 record_values->event_identifier );
		libcnotify_printf(
		 ")\n" );

		libcnotify_printf(
		 "%s: event type\t\t\t\t: %" PRIu16 " (",
		 function,
		 record_values->event_type );
		libevt_debug_print_event_type(
		 record_values->event_type );
		libcnotify_printf(
		 ")\n" );

		byte_stream_copy_to_uint16_little_endian(
		 ( (evt_record_event_header_t *) record_data )->number_of_strings,
		 value_16bit );
		libcnotify_printf(
		 "%s: number of strings\t\t\t: %" PRIu16 "\n",
		 function,
		 value_16bit );

		libcnotify_printf(
		 "%s: event category\t\t\t\t: %" PRIu16 "\n",
		 function,
		 record_values->event_category );

		byte_stream_copy_to_uint16_little_endian(
		 ( (evt_record_event_header_t *) record_data )->event_flags,
		 value_16bit );
		libcnotify_printf(
		 "%s: event flags\t\t\t\t: 0x%04" PRIx16 "\n",
		 function,
		 value_16bit );

		byte_stream_copy_to_uint32_little_endian(
		 ( (evt_record_event_header_t *) record_data )->closing_record_number,
		 value_32bit );
		libcnotify_printf(
		 "%s: closing record values number\t\t: %" PRIu32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: strings offset\t\t\t\t: %" PRIu32 "\n",
		 function,
		 strings_offset );

		libcnotify_printf(
		 "%s: user security identifier (SID) size\t: %" PRIu32 "\n",
		 function,
		 user_sid_size );

		libcnotify_printf(
		 "%s: user security identifier (SID) offset\t: %" PRIu32 "\n",
		 function,
		 user_sid_offset );

		libcnotify_printf(
		 "%s: data size\t\t\t\t: %" PRIu32 "\n",
		 function,
		 data_size );

		libcnotify_printf(
		 "%s: data offset\t\t\t\t: %" PRIu32 "\n",
		 function,
		 data_offset );
	}
#endif
	record_data_offset = sizeof( evt_record_event_header_t );

	if( ( user_sid_offset == 0 )
	 && ( user_sid_size != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: user SID offset or size value out of bounds.",
		 function );

		goto on_error;
	}
	if( user_sid_offset != 0 )
	{
		if( ( (size_t) user_sid_offset < record_data_offset )
		 || ( (size_t) user_sid_offset >= ( record_data_size - 4 ) ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: user SID offset value out of bounds.",
			 function );

			goto on_error;
		}
		if( user_sid_size != 0 )
		{
			if( (size_t) ( user_sid_offset + user_sid_size ) > ( record_data_size - 4 ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: user SID size value out of bounds.",
				 function );

				goto on_error;
			}
		}
	}
	/* If the strings offset is points at the offset at record data size - 4
	 * the strings are empty. For this to be sane the data offset should
	 * be the same as the strings offset or the data size 0.
	 */
	if( ( (size_t) strings_offset < user_sid_offset )
	 || ( (size_t) strings_offset >= ( record_data_size - 4 ) ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: strings offset value out of bounds.",
		 function );

		goto on_error;
	}
	if( ( (size_t) data_offset < strings_offset )
	 || ( (size_t) data_offset >= ( record_data_size - 4 ) ) )
	{
		if( data_size != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: data offset value out of bounds.",
			 function );

			goto on_error;
		}
		data_offset = (uint32_t) record_data_size - 4;
	}
	if( ( (size_t) strings_offset >= ( record_data_size - 4 ) )
	 && ( strings_offset != data_offset ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: strings offset value out of bounds.",
		 function );

		goto on_error;
	}
	if( strings_offset != 0 )
	{
		if( strings_offset < record_data_offset )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: strings offset value out of bounds.",
			 function );

			goto on_error;
		}
	}
	if( user_sid_offset != 0 )
	{
		members_data_size = user_sid_offset - (uint32_t) record_data_offset;
	}
	else if( strings_offset != 0 )
	{
		members_data_size = strings_offset - (uint32_t) record_data_offset;
	}
	if( strings_offset != 0 )
	{
		strings_size = data_offset - strings_offset;
	}
	if( data_size != 0 )
	{
		if( (size_t) ( data_offset + data_size ) > ( record_data_size - 4 ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: data size value out of bounds.",
			 function );

			goto on_error;
		}
	}
	if( members_data_size != 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: members data:\n",
			 function );
			libcnotify_print_data(
			 &( record_data[ record_data_offset ] ),
			 members_data_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
#endif
		if( libfvalue_value_type_initialize(
		     &( record_values->source_name ),
		     LIBFVALUE_VALUE_TYPE_STRING_UTF16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create source name value.",
			 function );

			goto on_error;
		}
		value_data_size = libfvalue_value_type_set_data_string(
		                   record_values->source_name,
		                   &( record_data[ record_data_offset ] ),
		                   members_data_size,
		                   LIBFVALUE_CODEPAGE_UTF16_LITTLE_ENDIAN,
		                   LIBFVALUE_VALUE_DATA_FLAG_MANAGED,
		                   error );

		if( value_data_size == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set data of source name value.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: source name\t\t\t\t: ",
			 function );

			if( libfvalue_value_print(
			     record_values->source_name,
			     0,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
				 "%s: unable to print source name value.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "\n" );
		}
#endif
		record_data_offset += value_data_size;
		members_data_size  -= (uint32_t) value_data_size;

		if( libfvalue_value_type_initialize(
		     &( record_values->computer_name ),
		     LIBFVALUE_VALUE_TYPE_STRING_UTF16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create computer name value.",
			 function );

			goto on_error;
		}
		value_data_size = libfvalue_value_type_set_data_string(
		                   record_values->computer_name,
		                   &( record_data[ record_data_offset ] ),
		                   members_data_size,
		                   LIBFVALUE_CODEPAGE_UTF16_LITTLE_ENDIAN,
		                   LIBFVALUE_VALUE_DATA_FLAG_MANAGED,
		                   error );

		if( value_data_size == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set data of computer name value.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: computer name\t\t\t\t: ",
			 function );

			if( libfvalue_value_print(
			     record_values->computer_name,
			     0,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
				 "%s: unable to print computer name value.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "\n" );
		}
#endif
		record_data_offset += value_data_size;
		members_data_size  -= (uint32_t) value_data_size;

		if( members_data_size > 0 )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: members trailing data:\n",
				 function );
				libcnotify_print_data(
				 &( record_data[ record_data_offset ] ),
				 members_data_size,
				 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
			}
#endif
			record_data_offset += members_data_size;
		}
 	}
 	if( user_sid_size != 0 )
 	{
		if( user_sid_size > ( ( record_data_size - 4 ) - user_sid_offset ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: user SID data size value out of bounds.",
			 function );

			goto on_error;
		}
 		if( libfvalue_value_type_initialize(
 		     &( record_values->user_security_identifier ),
 		     LIBFVALUE_VALUE_TYPE_NT_SECURITY_IDENTIFIER,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create user security identifier (SID) value.",
			 function );

			goto on_error;
		}
		if( libfvalue_value_set_data(
		     record_values->user_security_identifier,
		     &( record_data[ user_sid_offset ] ),
		     (size_t) user_sid_size,
		     LIBFVALUE_ENDIAN_LITTLE,
		     LIBFVALUE_VALUE_DATA_FLAG_MANAGED,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set data of user security identifier (SID) value.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: user security identifier (SID)\t\t: ",
			 function );

			if( libfvalue_value_print(
			     record_values->user_security_identifier,
			     0,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
				 "%s: unable to print user security identifier (SID) value.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "\n" );
		}
#endif
		record_data_offset += user_sid_size;
 	}
 	if( strings_size != 0 )
 	{
		if( strings_size > ( ( record_data_size - 4 ) - strings_offset ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: strings size value out of bounds.",
			 function );

			goto on_error;
		}
 #if defined( HAVE_DEBUG_OUTPUT )
 		if( libcnotify_verbose != 0 )
 		{
			libcnotify_printf(
			 "%s: strings data:\n",
			 function );
			libcnotify_print_data(
			 &( record_data[ strings_offset ] ),
			 strings_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
#endif
		if( size_copy == 0 )
		{
			/* If the strings data is truncated
			 */
			strings_data_offset = strings_offset + strings_size - 2;

			while( strings_data_offset > strings_offset )
			{
				if( ( record_data[ strings_data_offset ] != 0 )
				 || ( record_data[ strings_data_offset + 1 ] != 0 ) )
				{
					strings_size += 2;

					break;
				}
				strings_data_offset -= 2;
				strings_size        -= 2;
			}
		}
		if( libfvalue_value_type_initialize(
		     &( record_values->strings ),
		     LIBFVALUE_VALUE_TYPE_STRING_UTF16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create strings value.",
			 function );

			goto on_error;
		}
		value_data_size = libfvalue_value_type_set_data_strings_array(
		                   record_values->strings,
		                   &( record_data[ strings_offset ] ),
		                   strings_size,
		                   LIBFVALUE_CODEPAGE_UTF16_LITTLE_ENDIAN,
		                   error );

		if( value_data_size == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set data of strings value.",
			 function );

			goto on_error;
		}
		record_data_offset += strings_size;
 	}
 	if( data_size != 0 )
 	{
		if( data_size > ( ( record_data_size - 4 ) - data_offset ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: data size value out of bounds.",
			 function );

			goto on_error;
		}
 #if defined( HAVE_DEBUG_OUTPUT )
 		if( libcnotify_verbose != 0 )
 		{
 			libcnotify_printf(
 			 "%s: data:\n",
 			 function );
 			libcnotify_print_data(
			 &( record_data[ record_data_offset ] ),
 			 (size_t) data_size,
 			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
 		}
#endif
		if( libfvalue_value_type_initialize(
		     &( record_values->data ),
		     LIBFVALUE_VALUE_TYPE_BINARY_DATA,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create data value.",
			 function );

			goto on_error;
		}
		if( libfvalue_value_set_data(
		     record_values->data,
		     &( record_data[ record_data_offset ] ),
		     (size_t) data_size,
		     LIBFVALUE_ENDIAN_LITTLE,
		     LIBFVALUE_VALUE_DATA_FLAG_MANAGED,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set data of data value.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		record_data_offset += data_size;
#endif
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( record_data_offset < ( record_data_size - 4 ) )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 &( record_data[ record_data_offset ] ),
			 (size_t) record_data_size - record_data_offset - 4,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
		libcnotify_printf(
		 "%s: size copy\t\t\t\t: %" PRIu32 "\n",
		 function,
		 size_copy );

		libcnotify_printf(
		 "\n" );
	}
#endif
	if( ( strict_mode == 0 )
	 && ( size_copy == 0 ) )
	{
		size_copy = size;
	}
	if( size != size_copy )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_INPUT,
		 LIBCERROR_INPUT_ERROR_VALUE_MISMATCH,
		 "%s: value mismatch for size and size copy.",
		 function );

		goto on_error;
	}
	if( record_data_size != (size_t) size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_INPUT,
		 LIBCERROR_INPUT_ERROR_VALUE_MISMATCH,
		 "%s: value mismatch for record_values data size and size.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( record_values->data != NULL )
	{
		libfvalue_value_free(
		 &( record_values->data ),
		 NULL );
	}
	if( record_values->strings != NULL )
	{
		libfvalue_value_free(
		 &( record_values->strings ),
		 NULL );
	}
	if( record_values->user_security_identifier != NULL )
	{
		libfvalue_value_free(
		 &( record_values->user_security_identifier ),
		 NULL );
	}
	if( record_values->computer_name != NULL )
	{
		libfvalue_value_free(
		 &( record_values->computer_name ),
		 NULL );
	}
	if( record_values->source_name != NULL )
	{
		libfvalue_value_free(
		 &( record_values->source_name ),
		 NULL );
	}
	return( -1 );
}
