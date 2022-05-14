rdpdr_process(STREAM s)
{
	uint32 handle;
 	uint16 vmin;
 	uint16 component;
 	uint16 pakid;
 
 	logger(Protocol, Debug, "rdpdr_process()");
 	/* hexdump(s->p, s->end - s->p); */

	in_uint16(s, component);
	in_uint16(s, pakid);

	if (component == RDPDR_CTYP_CORE)
	{
		switch (pakid)
		{
			case PAKID_CORE_DEVICE_IOREQUEST:
				rdpdr_process_irp(s);
				break;

			case PAKID_CORE_SERVER_ANNOUNCE:
 				/* DR_CORE_SERVER_ANNOUNCE_REQ */
 				in_uint8s(s, 2);	/* skip versionMajor */
 				in_uint16_le(s, vmin);	/* VersionMinor */
 				in_uint32_le(s, g_client_id);	/* ClientID */
 
 				/* The RDP client is responsibility to provide a random client id
 				   if server version is < 12 */
 				if (vmin < 0x000c)
					g_client_id = 0x815ed39d;	/* IP address (use 127.0.0.1) 0x815ed39d */
				g_epoch++;

#if WITH_SCARD
				/*
				 * We need to release all SCARD contexts to end all
				 * current transactions and pending calls
				 */
				scard_release_all_contexts();

				/*
				 * According to [MS-RDPEFS] 3.2.5.1.2:
				 *
				 * If this packet appears after a sequence of other packets,
				 * it is a signal that the server has reconnected to a new session
				 * and the whole sequence has been reset. The client MUST treat
				 * this packet as the beginning of a new sequence.
				 * The client MUST also cancel all outstanding requests and release
				 * previous references to all devices.
				 *
				 * If any problem arises in the future, please, pay attention to the
				 * "If this packet appears after a sequence of other packets" part
				 *
				 */

#endif

				rdpdr_send_client_announce_reply();
				rdpdr_send_client_name_request();
				break;

			case PAKID_CORE_CLIENTID_CONFIRM:
				rdpdr_send_client_device_list_announce();
				break;

			case PAKID_CORE_DEVICE_REPLY:
				in_uint32(s, handle);
				logger(Protocol, Debug,
				       "rdpdr_process(), server connected to resource %d", handle);
				break;

			case PAKID_CORE_SERVER_CAPABILITY:
				rdpdr_send_client_capability_response();
				break;

			default:
				logger(Protocol, Debug,
				       "rdpdr_process(), pakid 0x%x of component 0x%x", pakid,
				       component);
				break;

		}
	}
	else if (component == RDPDR_CTYP_PRN)
	{
		if (pakid == PAKID_PRN_CACHE_DATA)
			printercache_process(s);
	}
	else
		logger(Protocol, Warning, "rdpdr_process(), unhandled component 0x%x", component);
}
