tBTA_AV_EVT bta_av_proc_meta_cmd(tAVRC_RESPONSE* p_rc_rsp,
                                 tBTA_AV_RC_MSG* p_msg, uint8_t* p_ctype) {
  tBTA_AV_EVT evt = BTA_AV_META_MSG_EVT;
 uint8_t u8, pdu, *p;
 uint16_t u16;
  tAVRC_MSG_VENDOR* p_vendor = &p_msg->msg.vendor;

  pdu = *(p_vendor->p_vendor_data);
  p_rc_rsp->pdu = pdu;
 *p_ctype = AVRC_RSP_REJ;

 /* Check to ansure a  valid minimum meta data length */
 if ((AVRC_MIN_META_CMD_LEN + p_vendor->vendor_len) > AVRC_META_CMD_BUF_SIZE) {
 /* reject it */
    p_rc_rsp->rsp.status = AVRC_STS_BAD_PARAM;
    APPL_TRACE_ERROR("%s: Invalid meta-command length: %d", __func__,
                     p_vendor->vendor_len);
 return 0;
 }

 /* Metadata messages only use PANEL sub-unit type */
 if (p_vendor->hdr.subunit_type != AVRC_SUB_PANEL) {
    APPL_TRACE_DEBUG("%s: SUBUNIT must be PANEL", __func__);
 /* reject it */
    evt = 0;
    p_vendor->hdr.ctype = AVRC_RSP_NOT_IMPL;
    p_vendor->vendor_len = 0;
    p_rc_rsp->rsp.status = AVRC_STS_BAD_PARAM;
 } else if (!AVRC_IsValidAvcType(pdu, p_vendor->hdr.ctype)) {
    APPL_TRACE_DEBUG("%s: Invalid pdu/ctype: 0x%x, %d", __func__, pdu,
                     p_vendor->hdr.ctype);
 /* reject invalid message without reporting to app */
    evt = 0;
    p_rc_rsp->rsp.status = AVRC_STS_BAD_CMD;
 } else {
 switch (pdu) {

       case AVRC_PDU_GET_CAPABILITIES:
         /* process GetCapabilities command without reporting the event to app */
         evt = 0;
        if (p_vendor->vendor_len != 5) {
          android_errorWriteLog(0x534e4554, "111893951");
          p_rc_rsp->get_caps.status = AVRC_STS_INTERNAL_ERR;
          break;
        }
         u8 = *(p_vendor->p_vendor_data + 4);
         p = p_vendor->p_vendor_data + 2;
         p_rc_rsp->get_caps.capability_id = u8;
         BE_STREAM_TO_UINT16(u16, p);
        if (u16 != 1) {
           p_rc_rsp->get_caps.status = AVRC_STS_INTERNAL_ERR;
         } else {
           p_rc_rsp->get_caps.status = AVRC_STS_NO_ERROR;
 if (u8 == AVRC_CAP_COMPANY_ID) {
 *p_ctype = AVRC_RSP_IMPL_STBL;
            p_rc_rsp->get_caps.count = p_bta_av_cfg->num_co_ids;
            memcpy(p_rc_rsp->get_caps.param.company_id,
                   p_bta_av_cfg->p_meta_co_ids,
 (p_bta_av_cfg->num_co_ids << 2));
 } else if (u8 == AVRC_CAP_EVENTS_SUPPORTED) {
 *p_ctype = AVRC_RSP_IMPL_STBL;
            p_rc_rsp->get_caps.count = p_bta_av_cfg->num_evt_ids;
            memcpy(p_rc_rsp->get_caps.param.event_id,
                   p_bta_av_cfg->p_meta_evt_ids, p_bta_av_cfg->num_evt_ids);
 } else {
            APPL_TRACE_DEBUG("%s: Invalid capability ID: 0x%x", __func__, u8);
 /* reject - unknown capability ID */
            p_rc_rsp->get_caps.status = AVRC_STS_BAD_PARAM;
 }
 }
 break;

 case AVRC_PDU_REGISTER_NOTIFICATION:
 /* make sure the event_id is implemented */
        p_rc_rsp->rsp.status = bta_av_chk_notif_evt_id(p_vendor);
 if (p_rc_rsp->rsp.status != BTA_AV_STS_NO_RSP) evt = 0;
 break;
 }
 }

 return evt;
}
