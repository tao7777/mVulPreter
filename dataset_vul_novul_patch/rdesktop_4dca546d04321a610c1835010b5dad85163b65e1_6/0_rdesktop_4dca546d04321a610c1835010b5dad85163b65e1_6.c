process_secondary_order(STREAM s)
{
	/* The length isn't calculated correctly by the server.
	 * For very compact orders the length becomes negative
	 * so a signed integer must be used. */
	uint16 length;
 	uint16 flags;
 	uint8 type;
 	uint8 *next_order;
	struct stream packet = *s;
 
 	in_uint16_le(s, length);
 	in_uint16_le(s, flags);	/* used by bmpcache2 */
 	in_uint8(s, type);
 
	if (!s_check_rem(s, length + 7))
	{
		rdp_protocol_error("process_secondary_order(), next order pointer would overrun stream", &packet);
	}

 	next_order = s->p + (sint16) length + 7;
 
 	switch (type)
	{
		case RDP_ORDER_RAW_BMPCACHE:
			process_raw_bmpcache(s);
			break;

		case RDP_ORDER_COLCACHE:
			process_colcache(s);
			break;

		case RDP_ORDER_BMPCACHE:
			process_bmpcache(s);
			break;

		case RDP_ORDER_FONTCACHE:
			process_fontcache(s);
			break;

		case RDP_ORDER_RAW_BMPCACHE2:
			process_bmpcache2(s, flags, False);	/* uncompressed */
			break;

		case RDP_ORDER_BMPCACHE2:
			process_bmpcache2(s, flags, True);	/* compressed */
			break;

		case RDP_ORDER_BRUSHCACHE:
			process_brushcache(s, flags);
			break;

		default:
			logger(Graphics, Warning,
			       "process_secondary_order(), unhandled secondary order %d", type);
	}

	s->p = next_order;
}
