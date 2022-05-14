void btm_sec_pin_code_request (UINT8 *p_bda)
{

     tBTM_SEC_DEV_REC *p_dev_rec;
     tBTM_CB          *p_cb = &btm_cb;
 
#ifdef PORCHE_PAIRING_CONFLICT
    UINT8 default_pin_code_len = 4;
    PIN_CODE default_pin_code = {0x30, 0x30, 0x30, 0x30};
#endif
     BTM_TRACE_EVENT ("btm_sec_pin_code_request()  State: %s, BDA:%04x%08x",
                       btm_pair_state_descr(btm_cb.pairing_state),
                       (p_bda[0]<<8)+p_bda[1], (p_bda[2]<<24)+(p_bda[3]<<16)+(p_bda[4]<<8)+p_bda[5] );

 if (btm_cb.pairing_state != BTM_PAIR_STATE_IDLE)
 {

         if ( (memcmp (p_bda, btm_cb.pairing_bda, BD_ADDR_LEN) == 0)  &&
              (btm_cb.pairing_state == BTM_PAIR_STATE_WAIT_AUTH_COMPLETE) )
         {
             /* fake this out - porshe carkit issue - */
             if(! btm_cb.pin_code_len_saved)
             {
                 btsnd_hcic_pin_code_neg_reply (p_bda);
                 return;
             }
             else
             {
                 btsnd_hcic_pin_code_req_reply (p_bda, btm_cb.pin_code_len_saved, p_cb->pin_code);
      	         return;
             }
         }
         else if ((btm_cb.pairing_state != BTM_PAIR_STATE_WAIT_PIN_REQ)
                  || memcmp (p_bda, btm_cb.pairing_bda, BD_ADDR_LEN) != 0)
 {

             BTM_TRACE_WARNING ("btm_sec_pin_code_request() rejected - state: %s",
                                 btm_pair_state_descr(btm_cb.pairing_state));
 
#ifdef PORCHE_PAIRING_CONFLICT
            /* reply pin code again due to counter in_rand when local initiates pairing */
            BTM_TRACE_EVENT ("btm_sec_pin_code_request from remote dev. for local initiated pairing");
            if(! btm_cb.pin_code_len_saved)
            {
                btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_AUTH_COMPLETE);
                btsnd_hcic_pin_code_req_reply (p_bda, default_pin_code_len, default_pin_code);
            }
            else
            {
                btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_AUTH_COMPLETE);
                btsnd_hcic_pin_code_req_reply (p_bda, btm_cb.pin_code_len_saved, p_cb->pin_code);
            }
#else
             btsnd_hcic_pin_code_neg_reply (p_bda);
#endif
             return;
         }
     }

    p_dev_rec = btm_find_or_alloc_dev (p_bda);
 /* received PIN code request. must be non-sm4 */
    p_dev_rec->sm4 = BTM_SM4_KNOWN;

 if (btm_cb.pairing_state == BTM_PAIR_STATE_IDLE)
 {
        memcpy (btm_cb.pairing_bda, p_bda, BD_ADDR_LEN);

        btm_cb.pairing_flags = BTM_PAIR_FLAGS_PEER_STARTED_DD;
 /* Make sure we reset the trusted mask to help against attacks */
        BTM_SEC_CLR_TRUSTED_DEVICE(p_dev_rec->trusted_mask);
 }

 if (!p_cb->pairing_disabled && (p_cb->cfg.pin_type == HCI_PIN_TYPE_FIXED))
 {
        BTM_TRACE_EVENT ("btm_sec_pin_code_request fixed pin replying");
        btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_AUTH_COMPLETE);
        btsnd_hcic_pin_code_req_reply (p_bda, p_cb->cfg.pin_code_len, p_cb->cfg.pin_code);
 return;
 }

 /* Use the connecting device's CoD for the connection */
 if ( (!memcmp (p_bda, p_cb->connecting_bda, BD_ADDR_LEN))
 && (p_cb->connecting_dc[0] || p_cb->connecting_dc[1] || p_cb->connecting_dc[2]) )
        memcpy (p_dev_rec->dev_class, p_cb->connecting_dc, DEV_CLASS_LEN);

 /* We could have started connection after asking user for the PIN code */
 if (btm_cb.pin_code_len != 0)
 {

         BTM_TRACE_EVENT ("btm_sec_pin_code_request bonding sending reply");
         btsnd_hcic_pin_code_req_reply (p_bda, btm_cb.pin_code_len, p_cb->pin_code);
 
#ifdef PORCHE_PAIRING_CONFLICT
        btm_cb.pin_code_len_saved = btm_cb.pin_code_len;
#endif
         /* Mark that we forwarded received from the user PIN code */
         btm_cb.pin_code_len = 0;
 
 /* We can change mode back right away, that other connection being established */
 /* is not forced to be secure - found a FW issue, so we can not do this
        btm_restore_mode(); */

        btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_AUTH_COMPLETE);
 }

 /* If pairing disabled OR (no PIN callback and not bonding) */
 /* OR we could not allocate entry in the database reject pairing request */
 else if (p_cb->pairing_disabled
 || (p_cb->api.p_pin_callback == NULL)

 /* OR Microsoft keyboard can for some reason try to establish connection */
 /*  the only thing we can do here is to shut it up.  Normally we will be originator */
 /*  for keyboard bonding */
 || (!p_dev_rec->is_originator
 && ((p_dev_rec->dev_class[1] & BTM_COD_MAJOR_CLASS_MASK) == BTM_COD_MAJOR_PERIPHERAL)
 && (p_dev_rec->dev_class[2] & BTM_COD_MINOR_KEYBOARD)) )
 {
        BTM_TRACE_WARNING("btm_sec_pin_code_request(): Pairing disabled:%d; PIN callback:%x, Dev Rec:%x!",
                           p_cb->pairing_disabled, p_cb->api.p_pin_callback, p_dev_rec);

        btsnd_hcic_pin_code_neg_reply (p_bda);
 }

     /* Notify upper layer of PIN request and start expiration timer */
     else
     {
        btm_cb.pin_code_len_saved = 0;
         btm_sec_change_pairing_state (BTM_PAIR_STATE_WAIT_LOCAL_PIN);
         /* Pin code request can not come at the same time as connection request */
         memcpy (p_cb->connecting_bda, p_bda, BD_ADDR_LEN);
        memcpy (p_cb->connecting_dc,  p_dev_rec->dev_class, DEV_CLASS_LEN);

 /* Check if the name is known */
 /* Even if name is not known we might not be able to get one */
 /* this is the case when we are already getting something from the */
 /* device, so HCI level is flow controlled */
 /* Also cannot send remote name request while paging, i.e. connection is not completed */
 if (p_dev_rec->sec_flags & BTM_SEC_NAME_KNOWN)
 {
            BTM_TRACE_EVENT ("btm_sec_pin_code_request going for callback");

            btm_cb.pairing_flags |= BTM_PAIR_FLAGS_PIN_REQD;
 if (p_cb->api.p_pin_callback)
 (*p_cb->api.p_pin_callback) (p_bda, p_dev_rec->dev_class, p_dev_rec->sec_bd_name);
 }
 else
 {
            BTM_TRACE_EVENT ("btm_sec_pin_code_request going for remote name");

 /* We received PIN code request for the device with unknown name */
 /* it is not user friendly just to ask for the PIN without name */
 /* try to get name at first */
 if (!btsnd_hcic_rmt_name_req (p_dev_rec->bd_addr,
                                          HCI_PAGE_SCAN_REP_MODE_R1,
                                          HCI_MANDATARY_PAGE_SCAN_MODE, 0))
 {
                p_dev_rec->sec_flags |= BTM_SEC_NAME_KNOWN;
                p_dev_rec->sec_bd_name[0] = 'f';
                p_dev_rec->sec_bd_name[1] = '0';
                BTM_TRACE_ERROR ("can not send rmt_name_req?? fake a name and call callback");

                btm_cb.pairing_flags |= BTM_PAIR_FLAGS_PIN_REQD;
 if (p_cb->api.p_pin_callback)
 (*p_cb->api.p_pin_callback) (p_bda, p_dev_rec->dev_class, p_dev_rec->sec_bd_name);
 }
 }
 }

 return;
}
