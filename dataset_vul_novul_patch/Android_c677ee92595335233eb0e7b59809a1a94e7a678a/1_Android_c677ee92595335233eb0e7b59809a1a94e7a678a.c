void BTM_PINCodeReply (BD_ADDR bd_addr, UINT8 res, UINT8 pin_len, UINT8 *p_pin, UINT32 trusted_mask[])
{
    tBTM_SEC_DEV_REC *p_dev_rec;

    BTM_TRACE_API ("BTM_PINCodeReply(): PairState: %s   PairFlags: 0x%02x  PinLen:%d  Result:%d",
                    btm_pair_state_descr(btm_cb.pairing_state), btm_cb.pairing_flags, pin_len, res);

 /* If timeout already expired or has been canceled, ignore the reply */
 if (btm_cb.pairing_state != BTM_PAIR_STATE_WAIT_LOCAL_PIN)
 {
        BTM_TRACE_WARNING ("BTM_PINCodeReply() - Wrong State: %d", btm_cb.pairing_state);
 return;
 }

 if (memcmp (bd_addr, btm_cb.pairing_bda, BD_ADDR_LEN) != 0)
 {
        BTM_TRACE_ERROR ("BTM_PINCodeReply() - Wrong BD Addr");
 return;
 }

 if ((p_dev_rec = btm_find_dev (bd_addr)) == NULL)
 {
        BTM_TRACE_ERROR ("BTM_PINCodeReply() - no dev CB");
 return;
 }

 if ( (pin_len > PIN_CODE_LEN) || (pin_len == 0) || (p_pin == NULL) )
        res = BTM_ILLEGAL_VALUE;

 if (res != BTM_SUCCESS)
 {
 /* if peer started dd OR we started dd and pre-fetch pin was not used send negative reply */
 if ((btm_cb.pairing_flags & BTM_PAIR_FLAGS_PEER_STARTED_DD) ||
 ((btm_cb.pairing_flags & BTM_PAIR_FLAGS_WE_STARTED_DD) &&
 (btm_cb.pairing_flags & BTM_PAIR_FLAGS_DISC_WHEN_DONE)) )
 {
 /* use BTM_PAIR_STATE_WAIT_AUTH_COMPLETE to report authentication failed event */
            btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_AUTH_COMPLETE);
            btm_cb.acl_disc_reason = HCI_ERR_HOST_REJECT_SECURITY;

            btsnd_hcic_pin_code_neg_reply (bd_addr);
 }
 else
 {
            p_dev_rec->security_required = BTM_SEC_NONE;
            btm_sec_change_pairing_state (BTM_PAIR_STATE_IDLE);
 }
 return;
 }
 if (trusted_mask)
        BTM_SEC_COPY_TRUSTED_DEVICE(trusted_mask, p_dev_rec->trusted_mask);
    p_dev_rec->sec_flags   |= BTM_SEC_LINK_KEY_AUTHED;

 if ( (btm_cb.pairing_flags & BTM_PAIR_FLAGS_WE_STARTED_DD)
 && (p_dev_rec->hci_handle == BTM_SEC_INVALID_HANDLE)
 && (btm_cb.security_mode_changed == FALSE) )
 {
 /* This is start of the dedicated bonding if local device is 2.0 */
        btm_cb.pin_code_len = pin_len;
        memcpy (btm_cb.pin_code, p_pin, pin_len);

        btm_cb.security_mode_changed = TRUE;
#ifdef APPL_AUTH_WRITE_EXCEPTION
 if(!(APPL_AUTH_WRITE_EXCEPTION)(p_dev_rec->bd_addr))
#endif
        btsnd_hcic_write_auth_enable (TRUE);

        btm_cb.acl_disc_reason = 0xff ;

 /* if we rejected incoming connection request, we have to wait HCI_Connection_Complete event */
 /*  before originating  */
 if (btm_cb.pairing_flags & BTM_PAIR_FLAGS_REJECTED_CONNECT)
 {
            BTM_TRACE_WARNING ("BTM_PINCodeReply(): waiting HCI_Connection_Complete after rejected incoming connection");
 /* we change state little bit early so btm_sec_connected() will originate connection */
 /*   when existing ACL link is down completely */
            btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_PIN_REQ);
 }
 /* if we already accepted incoming connection from pairing device */
 else if (p_dev_rec->sm4 & BTM_SM4_CONN_PEND)
 {
            BTM_TRACE_WARNING ("BTM_PINCodeReply(): link is connecting so wait pin code request from peer");
            btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_PIN_REQ);
 }
 else if (btm_sec_dd_create_conn(p_dev_rec) != BTM_CMD_STARTED)
 {
            btm_sec_change_pairing_state (BTM_PAIR_STATE_IDLE);
            p_dev_rec->sec_flags &= ~BTM_SEC_LINK_KEY_AUTHED;

 if (btm_cb.api.p_auth_complete_callback)
 (*btm_cb.api.p_auth_complete_callback) (p_dev_rec->bd_addr,  p_dev_rec->dev_class,
                                                    p_dev_rec->sec_bd_name, HCI_ERR_AUTH_FAILURE);
 }
 return;
 }


     btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_AUTH_COMPLETE);
     btm_cb.acl_disc_reason = HCI_SUCCESS;
 
#ifdef PORCHE_PAIRING_CONFLICT
    BTM_TRACE_EVENT("BTM_PINCodeReply(): Saving pin_len: %d btm_cb.pin_code_len: %d", pin_len, btm_cb.pin_code_len);
    /* if this was not pre-fetched, save the PIN */
    if (btm_cb.pin_code_len == 0)
        memcpy (btm_cb.pin_code, p_pin, pin_len);
    btm_cb.pin_code_len_saved = pin_len;
#endif
     btsnd_hcic_pin_code_req_reply (bd_addr, pin_len, p_pin);
 }
