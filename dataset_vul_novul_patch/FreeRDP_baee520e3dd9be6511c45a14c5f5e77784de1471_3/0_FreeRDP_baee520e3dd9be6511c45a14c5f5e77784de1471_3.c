static UINT drdynvc_process_data(drdynvcPlugin* drdynvc, int Sp, int cbChId,
                                  wStream* s)
 {
 	UINT32 ChannelId;

	if (Stream_GetRemainingLength(s) < drdynvc_cblen_to_bytes(cbChId))
		return ERROR_INVALID_DATA;

 	ChannelId = drdynvc_read_variable_uint(s, cbChId);
 	WLog_Print(drdynvc->log, WLOG_TRACE, "process_data: Sp=%d cbChId=%d, ChannelId=%"PRIu32"", Sp,
 	           cbChId,
	           ChannelId);
	return dvcman_receive_channel_data(drdynvc, drdynvc->channel_mgr, ChannelId, s);
}
