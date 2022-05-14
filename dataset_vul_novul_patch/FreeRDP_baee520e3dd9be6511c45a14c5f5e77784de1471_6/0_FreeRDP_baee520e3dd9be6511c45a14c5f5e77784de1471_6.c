static UINT dvcman_receive_channel_data(drdynvcPlugin* drdynvc,
                                        IWTSVirtualChannelManager* pChannelMgr,
                                        UINT32 ChannelId, wStream* data)
{
	UINT status = CHANNEL_RC_OK;
	DVCMAN_CHANNEL* channel;
	size_t dataSize = Stream_GetRemainingLength(data);
	channel = (DVCMAN_CHANNEL*) dvcman_find_channel_by_id(pChannelMgr, ChannelId);

	if (!channel)
	{
		/* Windows 8.1 tries to open channels not created.
				 * Ignore cases like this. */
		WLog_Print(drdynvc->log, WLOG_ERROR, "ChannelId %"PRIu32" not found!", ChannelId);
		return CHANNEL_RC_OK;
	}

 	if (channel->dvc_data)
 	{
 		/* Fragmented data */
		if (Stream_GetPosition(channel->dvc_data) + dataSize > Stream_Capacity(channel->dvc_data))
 		{
 			WLog_Print(drdynvc->log, WLOG_ERROR, "data exceeding declared length!");
 			Stream_Release(channel->dvc_data);
 			channel->dvc_data = NULL;
 			return ERROR_INVALID_DATA;
 		}
 
		Stream_Copy(data, channel->dvc_data, dataSize);
 
 		if (Stream_GetPosition(channel->dvc_data) >= channel->dvc_data_length)
 		{
			Stream_SealLength(channel->dvc_data);
			Stream_SetPosition(channel->dvc_data, 0);
			status = channel->channel_callback->OnDataReceived(channel->channel_callback,
			         channel->dvc_data);
			Stream_Release(channel->dvc_data);
			channel->dvc_data = NULL;
		}
	}
	else
	{
		status = channel->channel_callback->OnDataReceived(channel->channel_callback,
		         data);
	}

	return status;
}
