static UINT drdynvc_process_data_first(drdynvcPlugin* drdynvc, int Sp,
                                       int cbChId, wStream* s)
{
 	UINT status;
 	UINT32 Length;
 	UINT32 ChannelId;

	if (Stream_GetRemainingLength(s) < drdynvc_cblen_to_bytes(cbChId) + drdynvc_cblen_to_bytes(Sp))
		return ERROR_INVALID_DATA;

 	ChannelId = drdynvc_read_variable_uint(s, cbChId);
 	Length = drdynvc_read_variable_uint(s, Sp);
 	WLog_Print(drdynvc->log, WLOG_DEBUG,
	           "process_data_first: Sp=%d cbChId=%d, ChannelId=%"PRIu32" Length=%"PRIu32"", Sp,
	           cbChId, ChannelId, Length);
	status = dvcman_receive_channel_data_first(drdynvc, drdynvc->channel_mgr, ChannelId,
	         Length);

	if (status)
		return status;

	return dvcman_receive_channel_data(drdynvc, drdynvc->channel_mgr, ChannelId, s);
}
