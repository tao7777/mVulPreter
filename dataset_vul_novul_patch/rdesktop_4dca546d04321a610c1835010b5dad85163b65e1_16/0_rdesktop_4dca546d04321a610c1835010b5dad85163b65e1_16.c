sec_recv(RD_BOOL * is_fastpath)
{
	uint8 fastpath_hdr, fastpath_flags;
 	uint16 sec_flags;
 	uint16 channel;
 	STREAM s;
	struct stream packet;
 
 	while ((s = mcs_recv(&channel, is_fastpath, &fastpath_hdr)) != NULL)
 	{
		packet = *s;
 		if (*is_fastpath == True)
 		{
 			/* If fastpath packet is encrypted, read data
			   signature and decrypt */
			/* FIXME: extracting flags from hdr could be made less obscure */
 			fastpath_flags = (fastpath_hdr & 0xC0) >> 6;
 			if (fastpath_flags & FASTPATH_OUTPUT_ENCRYPTED)
 			{
				if (!s_check_rem(s, 8)) {
					rdp_protocol_error("sec_recv(), consume fastpath signature from stream would overrun", &packet);
				}

 				in_uint8s(s, 8);	/* signature */
 				sec_decrypt(s->p, s->end - s->p);
 			}
			return s;
		}

		if (g_encryption || (!g_licence_issued && !g_licence_error_result))
		{
			/* TS_SECURITY_HEADER */
			in_uint16_le(s, sec_flags);
			in_uint8s(s, 2);	/* skip sec_flags_hi */

			if (g_encryption)
 			{
 				if (sec_flags & SEC_ENCRYPT)
 				{
					if (!s_check_rem(s, 8)) {
						rdp_protocol_error("sec_recv(), consume encrypt signature from stream would overrun", &packet);
					}

 					in_uint8s(s, 8);	/* signature */
 					sec_decrypt(s->p, s->end - s->p);
 				}

				if (sec_flags & SEC_LICENSE_PKT)
				{
					licence_process(s);
					continue;
				}

				if (sec_flags & SEC_REDIRECTION_PKT)
 				{
 					uint8 swapbyte;
 
					if (!s_check_rem(s, 8)) {
						rdp_protocol_error("sec_recv(), consume redirect signature from stream would overrun", &packet);
					}

 					in_uint8s(s, 8);	/* signature */
 					sec_decrypt(s->p, s->end - s->p);
 
					/* Check for a redirect packet, starts with 00 04 */
					if (s->p[0] == 0 && s->p[1] == 4)
					{
						/* for some reason the PDU and the length seem to be swapped.
						   This isn't good, but we're going to do a byte for byte
						   swap.  So the first four value appear as: 00 04 XX YY,
						   where XX YY is the little endian length. We're going to
						   use 04 00 as the PDU type, so after our swap this will look
						   like: XX YY 04 00 */
						swapbyte = s->p[0];
						s->p[0] = s->p[2];
						s->p[2] = swapbyte;

						swapbyte = s->p[1];
						s->p[1] = s->p[3];
						s->p[3] = swapbyte;

						swapbyte = s->p[2];
						s->p[2] = s->p[3];
						s->p[3] = swapbyte;
					}
				}
			}
			else
			{
				if (sec_flags & SEC_LICENSE_PKT)
				{
					licence_process(s);
					continue;
				}
				s->p -= 4;
			}
		}

		if (channel != MCS_GLOBAL_CHANNEL)
		{
			channel_process(s, channel);
			continue;
		}

		return s;
	}

	return NULL;
}
