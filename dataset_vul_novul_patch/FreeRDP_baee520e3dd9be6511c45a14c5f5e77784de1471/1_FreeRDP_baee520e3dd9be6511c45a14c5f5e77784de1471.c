static UINT drdynvc_order_recv(drdynvcPlugin* drdynvc, wStream* s)
{
	int value;
 	int Cmd;
 	int Sp;
 	int cbChId;
 	Stream_Read_UINT8(s, value);
 	Cmd = (value & 0xf0) >> 4;
 	Sp = (value & 0x0c) >> 2;
	cbChId = (value & 0x03) >> 0;
	WLog_Print(drdynvc->log, WLOG_DEBUG, "order_recv: Cmd=0x%x, Sp=%d cbChId=%d", Cmd, Sp, cbChId);

	switch (Cmd)
	{
		case CAPABILITY_REQUEST_PDU:
			return drdynvc_process_capability_request(drdynvc, Sp, cbChId, s);

		case CREATE_REQUEST_PDU:
			return drdynvc_process_create_request(drdynvc, Sp, cbChId, s);

		case DATA_FIRST_PDU:
			return drdynvc_process_data_first(drdynvc, Sp, cbChId, s);

		case DATA_PDU:
			return drdynvc_process_data(drdynvc, Sp, cbChId, s);

		case CLOSE_REQUEST_PDU:
			return drdynvc_process_close_request(drdynvc, Sp, cbChId, s);

		default:
			WLog_Print(drdynvc->log, WLOG_ERROR, "unknown drdynvc cmd 0x%x", Cmd);
			return ERROR_INTERNAL_ERROR;
	}
}
