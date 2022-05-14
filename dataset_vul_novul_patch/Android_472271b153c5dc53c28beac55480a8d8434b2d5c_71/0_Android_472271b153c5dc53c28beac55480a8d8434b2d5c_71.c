BOOLEAN UIPC_Send(tUIPC_CH_ID ch_id, UINT16 msg_evt, UINT8 *p_buf,
        UINT16 msglen)
{
    UNUSED(msg_evt);

    BTIF_TRACE_DEBUG("UIPC_Send : ch_id:%d %d bytes", ch_id, msglen);

 
     UIPC_LOCK();
 
    if (TEMP_FAILURE_RETRY(write(uipc_main.ch[ch_id].fd, p_buf, msglen)) < 0)
     {
         BTIF_TRACE_ERROR("failed to write (%s)", strerror(errno));
     }

    UIPC_UNLOCK();

 return FALSE;
}
