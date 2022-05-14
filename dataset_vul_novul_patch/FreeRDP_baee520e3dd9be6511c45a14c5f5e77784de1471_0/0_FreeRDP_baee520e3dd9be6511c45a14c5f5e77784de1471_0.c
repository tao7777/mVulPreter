static UINT drdynvc_process_capability_request(drdynvcPlugin* drdynvc, int Sp,
        int cbChId, wStream* s)
{
	UINT status;

 	if (!drdynvc)
 		return CHANNEL_RC_BAD_INIT_HANDLE;
 
	if (Stream_GetRemainingLength(s) < 3)
		return ERROR_INVALID_DATA;

 	WLog_Print(drdynvc->log, WLOG_TRACE, "capability_request Sp=%d cbChId=%d", Sp, cbChId);
 	Stream_Seek(s, 1); /* pad */
 	Stream_Read_UINT16(s, drdynvc->version);

	/* RDP8 servers offer version 3, though Microsoft forgot to document it
	 * in their early documents.  It behaves the same as version 2.
 	 */
 	if ((drdynvc->version == 2) || (drdynvc->version == 3))
 	{
		if (Stream_GetRemainingLength(s) < 8)
			return ERROR_INVALID_DATA;

 		Stream_Read_UINT16(s, drdynvc->PriorityCharge0);
 		Stream_Read_UINT16(s, drdynvc->PriorityCharge1);
 		Stream_Read_UINT16(s, drdynvc->PriorityCharge2);
		Stream_Read_UINT16(s, drdynvc->PriorityCharge3);
	}

	status = drdynvc_send_capability_response(drdynvc);
	drdynvc->state = DRDYNVC_STATE_READY;
 	return status;
 }
