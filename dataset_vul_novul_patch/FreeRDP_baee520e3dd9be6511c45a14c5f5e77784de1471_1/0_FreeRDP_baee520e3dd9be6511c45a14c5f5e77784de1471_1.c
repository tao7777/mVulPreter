static UINT drdynvc_process_close_request(drdynvcPlugin* drdynvc, int Sp,
        int cbChId, wStream* s)
{
	int value;
 	UINT error;
 	UINT32 ChannelId;
 	wStream* data_out;

	if (Stream_GetRemainingLength(s) < drdynvc_cblen_to_bytes(cbChId))
		return ERROR_INVALID_DATA;

 	ChannelId = drdynvc_read_variable_uint(s, cbChId);
 	WLog_Print(drdynvc->log, WLOG_DEBUG, "process_close_request: Sp=%d cbChId=%d, ChannelId=%"PRIu32"",
 	           Sp,
	           cbChId, ChannelId);

	if ((error = dvcman_close_channel(drdynvc->channel_mgr, ChannelId)))
	{
		WLog_Print(drdynvc->log, WLOG_ERROR, "dvcman_close_channel failed with error %"PRIu32"!", error);
		return error;
	}

	data_out = Stream_New(NULL, 4);

	if (!data_out)
	{
		WLog_Print(drdynvc->log, WLOG_ERROR, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	value = (CLOSE_REQUEST_PDU << 4) | (cbChId & 0x03);
	Stream_Write_UINT8(data_out, value);
	drdynvc_write_variable_uint(data_out, ChannelId);
	error = drdynvc_send(drdynvc, data_out);

	if (error)
		WLog_Print(drdynvc->log, WLOG_ERROR, "VirtualChannelWriteEx failed with %s [%08"PRIX32"]",
		           WTSErrorToString(error), error);

	return error;
}
