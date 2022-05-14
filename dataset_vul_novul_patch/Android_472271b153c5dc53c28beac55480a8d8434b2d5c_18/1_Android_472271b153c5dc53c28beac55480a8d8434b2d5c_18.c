void btif_hl_select_monitor_callback(fd_set *p_cur_set ,fd_set *p_org_set) {
    UNUSED(p_org_set);

    BTIF_TRACE_DEBUG("entering %s",__FUNCTION__);

 for (const list_node_t *node = list_begin(soc_queue);
            node != list_end(soc_queue); node = list_next(node)) {
 btif_hl_soc_cb_t *p_scb = list_node(node);
 if (btif_hl_get_socket_state(p_scb) == BTIF_HL_SOC_STATE_W4_READ) {
 if (FD_ISSET(p_scb->socket_id[1], p_cur_set)) {
                BTIF_TRACE_DEBUG("read data state= BTIF_HL_SOC_STATE_W4_READ");
 btif_hl_mdl_cb_t *p_dcb = BTIF_HL_GET_MDL_CB_PTR(p_scb->app_idx,
                        p_scb->mcl_idx, p_scb->mdl_idx);
                assert(p_dcb != NULL);
 if (p_dcb->p_tx_pkt) {
                    BTIF_TRACE_ERROR("Rcv new pkt but the last pkt is still not been"
 "  sent tx_size=%d", p_dcb->tx_size);
                    btif_hl_free_buf((void **) &p_dcb->p_tx_pkt);

                 }
                 p_dcb->p_tx_pkt = btif_hl_get_buf (p_dcb->mtu);
                 if (p_dcb) {
                    int r = (int)recv(p_scb->socket_id[1], p_dcb->p_tx_pkt,
                            p_dcb->mtu, MSG_DONTWAIT);
                     if (r > 0) {
                         BTIF_TRACE_DEBUG("btif_hl_select_monitor_callback send data r =%d", r);
                         p_dcb->tx_size = r;
                        BTIF_TRACE_DEBUG("btif_hl_select_monitor_callback send data tx_size=%d", p_dcb->tx_size );
                        BTA_HlSendData(p_dcb->mdl_handle, p_dcb->tx_size);
 } else {
                        BTIF_TRACE_DEBUG("btif_hl_select_monitor_callback receive failed r=%d",r);
                        BTA_HlDchClose(p_dcb->mdl_handle);
 }
 }
 }
 }
 }

 if (list_is_empty(soc_queue))
        BTIF_TRACE_DEBUG("btif_hl_select_monitor_queue is empty");

    BTIF_TRACE_DEBUG("leaving %s",__FUNCTION__);
}
