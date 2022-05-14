void bta_hl_co_put_rx_data (UINT8 app_id, tBTA_HL_MDL_HANDLE mdl_handle,
                            UINT16 data_size, UINT8 *p_data, UINT16 evt)
{
    UINT8 app_idx, mcl_idx, mdl_idx;
 btif_hl_mdl_cb_t *p_dcb;
    tBTA_HL_STATUS status = BTA_HL_STATUS_FAIL;
 int            r;
    BTIF_TRACE_DEBUG("%s app_id=%d mdl_handle=0x%x data_size=%d",
                      __FUNCTION__,app_id, mdl_handle, data_size);

 if (btif_hl_find_mdl_idx_using_handle(mdl_handle, &app_idx, &mcl_idx, &mdl_idx))
 {
        p_dcb = BTIF_HL_GET_MDL_CB_PTR(app_idx, mcl_idx, mdl_idx);

 if ((p_dcb->p_rx_pkt = (UINT8 *)btif_hl_get_buf(data_size)) != NULL)
 {
            memcpy(p_dcb->p_rx_pkt, p_data, data_size);
 if (p_dcb->p_scb)

             {
                 BTIF_TRACE_DEBUG("app_idx=%d mcl_idx=0x%x mdl_idx=0x%x data_size=%d",
                                   app_idx, mcl_idx, mdl_idx, data_size);
                r = TEMP_FAILURE_RETRY(send(p_dcb->p_scb->socket_id[1], p_dcb->p_rx_pkt, data_size, 0));
 
                 if (r == data_size)
                 {
                    BTIF_TRACE_DEBUG("socket send success data_size=%d",  data_size);
                    status = BTA_HL_STATUS_OK;
 }
 else
 {
                    BTIF_TRACE_ERROR("socket send failed r=%d data_size=%d",r, data_size);
 }


 }
            btif_hl_free_buf((void **) &p_dcb->p_rx_pkt);
 }
 }

    bta_hl_ci_put_rx_data(mdl_handle,  status, evt);
}
