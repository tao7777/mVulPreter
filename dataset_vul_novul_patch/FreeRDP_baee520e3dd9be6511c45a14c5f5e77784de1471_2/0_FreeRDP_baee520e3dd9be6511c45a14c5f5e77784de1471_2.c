static UINT drdynvc_process_create_request(drdynvcPlugin* drdynvc, int Sp,
        int cbChId, wStream* s)
{
	size_t pos;
	UINT status;
 	UINT32 ChannelId;
 	wStream* data_out;
 	UINT channel_status;
	char* name;
	size_t length;
 
 	if (!drdynvc)
 		return CHANNEL_RC_BAD_CHANNEL_HANDLE;

	if (drdynvc->state == DRDYNVC_STATE_CAPABILITIES)
	{
		/**
		 * For some reason the server does not always send the
		 * capabilities pdu as it should. When this happens,
		 * send a capabilities response.
		 */
		drdynvc->version = 3;

		if ((status = drdynvc_send_capability_response(drdynvc)))
		{
			WLog_Print(drdynvc->log, WLOG_ERROR, "drdynvc_send_capability_response failed!");
			return status;
		}

 		drdynvc->state = DRDYNVC_STATE_READY;
 	}
 
	if (Stream_GetRemainingLength(s) < drdynvc_cblen_to_bytes(cbChId))
		return ERROR_INVALID_DATA;

 	ChannelId = drdynvc_read_variable_uint(s, cbChId);
 	pos = Stream_GetPosition(s);
	name = Stream_Pointer(s);
	length = Stream_GetRemainingLength(s);

	if (strnlen(name, length) >= length)
		return ERROR_INVALID_DATA;

 	WLog_Print(drdynvc->log, WLOG_DEBUG, "process_create_request: ChannelId=%"PRIu32" ChannelName=%s",
	           ChannelId, name);
	channel_status = dvcman_create_channel(drdynvc, drdynvc->channel_mgr, ChannelId, name);
 	data_out = Stream_New(NULL, pos + 4);
 
 	if (!data_out)
	{
		WLog_Print(drdynvc->log, WLOG_ERROR, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Write_UINT8(data_out, 0x10 | cbChId);
	Stream_SetPosition(s, 1);
	Stream_Copy(s, data_out, pos - 1);

	if (channel_status == CHANNEL_RC_OK)
	{
		WLog_Print(drdynvc->log, WLOG_DEBUG, "channel created");
		Stream_Write_UINT32(data_out, 0);
	}
	else
	{
		WLog_Print(drdynvc->log, WLOG_DEBUG, "no listener");
		Stream_Write_UINT32(data_out, (UINT32)0xC0000001); /* same code used by mstsc */
	}

	status = drdynvc_send(drdynvc, data_out);

	if (status != CHANNEL_RC_OK)
	{
		WLog_Print(drdynvc->log, WLOG_ERROR, "VirtualChannelWriteEx failed with %s [%08"PRIX32"]",
		           WTSErrorToString(status), status);
		return status;
	}

	if (channel_status == CHANNEL_RC_OK)
	{
		if ((status = dvcman_open_channel(drdynvc, drdynvc->channel_mgr, ChannelId)))
		{
			WLog_Print(drdynvc->log, WLOG_ERROR, "dvcman_open_channel failed with error %"PRIu32"!", status);
			return status;
		}
	}
	else
	{
		if ((status = dvcman_close_channel(drdynvc->channel_mgr, ChannelId)))
			WLog_Print(drdynvc->log, WLOG_ERROR, "dvcman_close_channel failed with error %"PRIu32"!", status);
	}

	return status;
}
