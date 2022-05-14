static UINT drdynvc_virtual_channel_event_data_received(drdynvcPlugin* drdynvc,
        void* pData, UINT32 dataLength, UINT32 totalLength, UINT32 dataFlags)
{
	wStream* data_in;

	if ((dataFlags & CHANNEL_FLAG_SUSPEND) || (dataFlags & CHANNEL_FLAG_RESUME))
	{
		return CHANNEL_RC_OK;
	}

	if (dataFlags & CHANNEL_FLAG_FIRST)
	{
		if (drdynvc->data_in)
			Stream_Free(drdynvc->data_in, TRUE);

		drdynvc->data_in = Stream_New(NULL, totalLength);
	}

	if (!(data_in = drdynvc->data_in))
	{
		WLog_Print(drdynvc->log, WLOG_ERROR, "Stream_New failed!");
 		return CHANNEL_RC_NO_MEMORY;
 	}
 
	if (!Stream_EnsureRemainingCapacity(data_in, dataLength))
 	{
 		WLog_Print(drdynvc->log, WLOG_ERROR, "Stream_EnsureRemainingCapacity failed!");
 		Stream_Free(drdynvc->data_in, TRUE);
		drdynvc->data_in = NULL;
		return ERROR_INTERNAL_ERROR;
	}

	Stream_Write(data_in, pData, dataLength);

	if (dataFlags & CHANNEL_FLAG_LAST)
	{
		if (Stream_Capacity(data_in) != Stream_GetPosition(data_in))
		{
			WLog_Print(drdynvc->log, WLOG_ERROR, "drdynvc_plugin_process_received: read error");
			return ERROR_INVALID_DATA;
		}

		drdynvc->data_in = NULL;
		Stream_SealLength(data_in);
		Stream_SetPosition(data_in, 0);

		if (!MessageQueue_Post(drdynvc->queue, NULL, 0, (void*) data_in, NULL))
		{
			WLog_Print(drdynvc->log, WLOG_ERROR, "MessageQueue_Post failed!");
			return ERROR_INTERNAL_ERROR;
		}
	}

	return CHANNEL_RC_OK;
}
