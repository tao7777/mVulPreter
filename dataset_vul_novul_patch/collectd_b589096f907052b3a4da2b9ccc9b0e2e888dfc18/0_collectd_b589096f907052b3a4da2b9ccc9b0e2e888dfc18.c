static int parse_packet (sockent_t *se, /* {{{ */
		void *buffer, size_t buffer_size, int flags,
		const char *username)
{
	int status;

	value_list_t vl = VALUE_LIST_INIT;
	notification_t n;

#if HAVE_LIBGCRYPT
	int packet_was_signed = (flags & PP_SIGNED);
        int packet_was_encrypted = (flags & PP_ENCRYPTED);
	int printed_ignore_warning = 0;
#endif /* HAVE_LIBGCRYPT */


	memset (&vl, '\0', sizeof (vl));
	memset (&n, '\0', sizeof (n));
	status = 0;

	while ((status == 0) && (0 < buffer_size)
			&& ((unsigned int) buffer_size > sizeof (part_header_t)))
	{
		uint16_t pkg_length;
		uint16_t pkg_type;

		memcpy ((void *) &pkg_type,
				(void *) buffer,
				sizeof (pkg_type));
		memcpy ((void *) &pkg_length,
				(void *) (buffer + sizeof (pkg_type)),
				sizeof (pkg_length));

		pkg_length = ntohs (pkg_length);
		pkg_type = ntohs (pkg_type);

		if (pkg_length > buffer_size)
			break;
		/* Ensure that this loop terminates eventually */
		if (pkg_length < (2 * sizeof (uint16_t)))
			break;

		if (pkg_type == TYPE_ENCR_AES256)
		{
			status = parse_part_encr_aes256 (se,
					&buffer, &buffer_size, flags);
			if (status != 0)
			{
				ERROR ("network plugin: Decrypting AES256 "
						"part failed "
						"with status %i.", status);
				break;
			}
		}
#if HAVE_LIBGCRYPT
		else if ((se->data.server.security_level == SECURITY_LEVEL_ENCRYPT)
				&& (packet_was_encrypted == 0))
		{
			if (printed_ignore_warning == 0)
			{
				INFO ("network plugin: Unencrypted packet or "
						"part has been ignored.");
 				printed_ignore_warning = 1;
 			}
 			buffer = ((char *) buffer) + pkg_length;
			buffer_size -= (size_t) pkg_length;
 			continue;
 		}
 #endif /* HAVE_LIBGCRYPT */
		else if (pkg_type == TYPE_SIGN_SHA256)
		{
			status = parse_part_sign_sha256 (se,
                                        &buffer, &buffer_size, flags);
			if (status != 0)
			{
				ERROR ("network plugin: Verifying HMAC-SHA-256 "
						"signature failed "
						"with status %i.", status);
				break;
			}
		}
#if HAVE_LIBGCRYPT
		else if ((se->data.server.security_level == SECURITY_LEVEL_SIGN)
				&& (packet_was_encrypted == 0)
				&& (packet_was_signed == 0))
		{
			if (printed_ignore_warning == 0)
			{
				INFO ("network plugin: Unsigned packet or "
						"part has been ignored.");
 				printed_ignore_warning = 1;
 			}
 			buffer = ((char *) buffer) + pkg_length;
			buffer_size -= (size_t) pkg_length;
 			continue;
 		}
 #endif /* HAVE_LIBGCRYPT */
		else if (pkg_type == TYPE_VALUES)
		{
			status = parse_part_values (&buffer, &buffer_size,
					&vl.values, &vl.values_len);
			if (status != 0)
				break;

			network_dispatch_values (&vl, username);

			sfree (vl.values);
		}
		else if (pkg_type == TYPE_TIME)
		{
			uint64_t tmp = 0;
			status = parse_part_number (&buffer, &buffer_size,
					&tmp);
			if (status == 0)
			{
				vl.time = TIME_T_TO_CDTIME_T (tmp);
				n.time  = TIME_T_TO_CDTIME_T (tmp);
			}
		}
		else if (pkg_type == TYPE_TIME_HR)
		{
			uint64_t tmp = 0;
			status = parse_part_number (&buffer, &buffer_size,
					&tmp);
			if (status == 0)
			{
				vl.time = (cdtime_t) tmp;
				n.time  = (cdtime_t) tmp;
			}
		}
		else if (pkg_type == TYPE_INTERVAL)
		{
			uint64_t tmp = 0;
			status = parse_part_number (&buffer, &buffer_size,
					&tmp);
			if (status == 0)
				vl.interval = TIME_T_TO_CDTIME_T (tmp);
		}
		else if (pkg_type == TYPE_INTERVAL_HR)
		{
			uint64_t tmp = 0;
			status = parse_part_number (&buffer, &buffer_size,
					&tmp);
			if (status == 0)
				vl.interval = (cdtime_t) tmp;
		}
		else if (pkg_type == TYPE_HOST)
		{
			status = parse_part_string (&buffer, &buffer_size,
					vl.host, sizeof (vl.host));
			if (status == 0)
				sstrncpy (n.host, vl.host, sizeof (n.host));
		}
		else if (pkg_type == TYPE_PLUGIN)
		{
			status = parse_part_string (&buffer, &buffer_size,
					vl.plugin, sizeof (vl.plugin));
			if (status == 0)
				sstrncpy (n.plugin, vl.plugin,
						sizeof (n.plugin));
		}
		else if (pkg_type == TYPE_PLUGIN_INSTANCE)
		{
			status = parse_part_string (&buffer, &buffer_size,
					vl.plugin_instance,
					sizeof (vl.plugin_instance));
			if (status == 0)
				sstrncpy (n.plugin_instance,
						vl.plugin_instance,
						sizeof (n.plugin_instance));
		}
		else if (pkg_type == TYPE_TYPE)
		{
			status = parse_part_string (&buffer, &buffer_size,
					vl.type, sizeof (vl.type));
			if (status == 0)
				sstrncpy (n.type, vl.type, sizeof (n.type));
		}
		else if (pkg_type == TYPE_TYPE_INSTANCE)
		{
			status = parse_part_string (&buffer, &buffer_size,
					vl.type_instance,
					sizeof (vl.type_instance));
			if (status == 0)
				sstrncpy (n.type_instance, vl.type_instance,
						sizeof (n.type_instance));
		}
		else if (pkg_type == TYPE_MESSAGE)
		{
			status = parse_part_string (&buffer, &buffer_size,
					n.message, sizeof (n.message));

			if (status != 0)
			{
				/* do nothing */
			}
			else if ((n.severity != NOTIF_FAILURE)
					&& (n.severity != NOTIF_WARNING)
					&& (n.severity != NOTIF_OKAY))
			{
				INFO ("network plugin: "
						"Ignoring notification with "
						"unknown severity %i.",
						n.severity);
			}
			else if (n.time <= 0)
			{
				INFO ("network plugin: "
						"Ignoring notification with "
						"time == 0.");
			}
			else if (strlen (n.message) <= 0)
			{
				INFO ("network plugin: "
						"Ignoring notification with "
						"an empty message.");
			}
			else
			{
				network_dispatch_notification (&n);
			}
		}
		else if (pkg_type == TYPE_SEVERITY)
		{
			uint64_t tmp = 0;
			status = parse_part_number (&buffer, &buffer_size,
					&tmp);
			if (status == 0)
				n.severity = (int) tmp;
		}
		else
		{
 			DEBUG ("network plugin: parse_packet: Unknown part"
 					" type: 0x%04hx", pkg_type);
 			buffer = ((char *) buffer) + pkg_length;
			buffer_size -= (size_t) pkg_length;
 		}
 	} /* while (buffer_size > sizeof (part_header_t)) */
 
	if (status == 0 && buffer_size > 0)
		WARNING ("network plugin: parse_packet: Received truncated "
				"packet, try increasing `MaxPacketSize'");

	return (status);
} /* }}} int parse_packet */
